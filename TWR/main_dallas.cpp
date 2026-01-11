#include "main.h"
#include "main_server.h"

#include "devDataSetConfig.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <utilsDALLAS.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace dallas = utils::dallas;

dallas::tID ToDallasID(std::string& keyIDStr)
{
	if (keyIDStr.size() != 12) // 112233445566
		return {};
	auto KeyIDStrIter = keyIDStr.begin();
	dallas::tID KeyID{};
	for (std::size_t i = 0, j = 5; i < 6; ++i, --j) // sizeof(dallas::tID::Value) = 6
	{
		KeyID.Value[j] = utils::Read<std::uint8_t>(KeyIDStrIter, KeyIDStrIter + 2, utils::tRadix::hex);
		KeyIDStrIter += 2;
	}
	return KeyID;
}

//std::string ToString(const dallas::tROM& rom)
//{
//	std::stringstream SStr;
//	std::for_each(std::begin(rom.Value), std::end(rom.Value), [&SStr](std::uint8_t val) { SStr << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(val); });
//	return SStr.str();
//};

std::string ToString(const dallas::tFamilyCode& val)
{
	std::stringstream SStr;
	SStr << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(val);
	return SStr.str();
};

std::string ToString(const dallas::tID& id)
{
	std::stringstream SStr;
	std::for_each(std::rbegin(id.Value), std::rend(id.Value), [&SStr](std::uint8_t val) { SStr << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(val); });
	return SStr.str();
};

//std::string ToString(dallas::tCodeError codeError)
//{
//	switch (codeError)
//	{
//	case dallas::tCodeError::Success: return "ok";
//	case dallas::tCodeError::Connection: return "Key is not connected";
//	case dallas::tCodeError::SameKeyID: return "Key ID is the same";
//	case dallas::tCodeError::NotWritten: return "Key ID is not written";
//	case dallas::tCodeError::WrongKeyID: return "Key ID is wrong";
//	}
//	return "unknown";
//}

//enum class tCodeError
//{
//	Success,
//	Connection,
//	SameKeyID,
//	NotWritten,
//	WrongKeyID,
//};

void ThreadDALLAS(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	using tPortHld = tPortHolder<dallas::tDALLAS>;

	try
	{
		std::weak_ptr<dev::tDataSetConfig> ConfigWeak(config);

		std::string PortID;
		{
			std::shared_ptr<dev::tDataSetConfig> Config = ConfigWeak.lock();
			PortID = config->GetDallas().ID;
			if (PortID.empty())
				return; // [TBD] throw an exception
		}

		tTWRQueueUARTCmd& QueueIn = TWRQueue.DALLAS;

		std::unique_ptr<tPortHld> PortPtr;

		while (!ConfigWeak.expired())
		{
			if (QueueIn.empty())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(!PortPtr ? 100 : 1));
				continue;
			}

			tPack Pack = QueueIn.get_front();
			std::stringstream SStr(std::move(Pack.Value));

			try
			{
				boost::property_tree::ptree PTree;
				boost::property_tree::json_parser::read_json(SStr, PTree);
				std::string Cmd = PTree.get<std::string>("cmd");

				if (Cmd == "open")
				{
					if (PortPtr)
						PortPtr.reset();
					PortPtr = std::make_unique<tPortHld>(PortID, dallas::tDALLAS::tSpeed::Norm);
					server.SendResponse(Pack.Endpoint, PTree, "ok");
					continue;
				}
				else if (Cmd == "close") // It's just a response, nothing more (for compatibility reasons).
				{
					PortPtr.reset();
					server.SendResponse(Pack.Endpoint, PTree, "ok");
					continue;
				}

				if (!PortPtr)
					continue;

				if ((*PortPtr)().Reset() != dallas::tDALLAS::tStatus::Success)
				{
					server.SendResponse(Pack.Endpoint, PTree, "not connected");
					continue;
				}

				if (Cmd == "search")
				{
					std::string FamilyCodeStr = PTree.get<std::string>("family_code", "0");
					dallas::tFamilyCode FamilyCode = static_cast<dallas::tFamilyCode>(std::strtoul(FamilyCodeStr.c_str(), nullptr, 16));

					std::vector<dallas::tROM> FoundROMs;
					if (FamilyCode == dallas::tFamilyCode::None)
					{
						FoundROMs = (*PortPtr)().Search();
					}
					else
					{
						FoundROMs = (*PortPtr)().Search(FamilyCode);
					}

					if (!FoundROMs.empty())
					{
						boost::property_tree::ptree PTreeChildren;
						for (const auto& i : FoundROMs)
						{
							boost::property_tree::ptree PTreeChild;
							PTreeChild.put("family_code", ToString(i.Field.FamilyCode));
							PTreeChild.put("id", ToString(i.Field.ID));
							PTreeChildren.push_back(std::make_pair("", PTreeChild));
						}
						PTree.add_child("roms", PTreeChildren);
					}

					server.SendResponse(Pack.Endpoint, PTree, "ok");
				}
				//else if (Cmd == "read_key")
				//{
				//	std::optional<dallas::tID> KeyID = Port.ReadKey();
				//	if (KeyID.has_value())
				//	{
				//		PTree.put("id", ToString(*KeyID));
				//		SendRsp(server, Pack.Endpoint, PTree, "ok");
				//	}
				//	else
				//	{
				//		SendRsp(server, Pack.Endpoint, PTree, "no key");
				//	}
				//}
				//else if (Cmd == "write_key")
				//{
				//	std::string KeyIDStr = PTree.get<std::string>("id", "");
				//	if (KeyIDStr.size() == 12) // 112233445566
				//	{
				//		dallas::tID KeyID = ToDallasID(KeyIDStr);
				//		const dallas::tCodeError CodeError = Port.WriteKey(KeyID);
				//		SendRsp(server, Pack.Endpoint, PTree, ToString(CodeError));
				//	}
				//	else
				//	{
				//		SendRsp(server, Pack.Endpoint, PTree, "wrong key id");
				//	}
				//}
				//else if (Cmd == "read_rom")
				//{
				//	SendRsp(server, Pack.Endpoint, PTree, "not supported");
				//}
				else if (Cmd == "thermo")
				{
					std::vector<dallas::tROM> ROMs;
					for (auto& msg : PTree.get_child("ids"))
					{
						std::string IDStr = msg.second.get_value<std::string>();
						dallas::tID ID = ToDallasID(IDStr);
						dallas::tROM ROM = MakeROM(dallas::tFamilyCode::DS18B20, ID);
						ROMs.push_back(ROM);
					}

					if (ROMs.empty())
					{
						server.SendResponse(Pack.Endpoint, PTree, "no id");
						continue;
					}

					std::vector<dallas::tDsDS18B20> Thermo = (*PortPtr)().GetDsDS18B20(ROMs);
					if (Thermo.empty())
					{
						server.SendResponse(Pack.Endpoint, PTree, "no measurements");
						continue;
					}

					boost::property_tree::ptree PTreeChildren;
					for (const auto& i : Thermo)
					{
						boost::property_tree::ptree PTreeChild;
						PTreeChild.put("id", ToString(i.ROM.Field.ID));
						PTreeChild.put("temperature", i.Temperature);
						PTreeChildren.push_back(std::make_pair("", PTreeChild));
					}
					PTree.add_child("measurements", PTreeChildren);
					server.SendResponse(Pack.Endpoint, PTree, "ok");
				}
				else
				{
					server.SendResponse(Pack.Endpoint, PTree, "unknown");
				}
			}
			catch (const boost::system::system_error& ex)
			{
				PortPtr.reset();
				std::cerr << ex.what() << '\n';
			}
			catch (...) {} // Format JSON shall be verified in the receiver (e.g. in UDP-Server)
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
