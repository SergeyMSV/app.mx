#include "main.h"

#include "devDataSet.h"
#include "devDataSetConfig.h"

#include <future>
#include <iomanip>
#include <memory>
#include <sstream>

#include <cstdlib>

#include <utilsBase.h>
#include <utilsDALLAS.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace dallas = utils::DALLAS;
/*
dallas::tID ToDallasID(std::string& keyIDStr)
{
	if (keyIDStr.size() != 12) // 00000308c4de
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

std::string ToString(dallas::tFamilyCode val)
{
	std::stringstream SStr;
	SStr << std::setfill('0') << std::setw(2) << std::hex << (int)val;
	return SStr.str();
};

std::string ToString(const dallas::tROM& rom)
{
	std::stringstream SStr;
	std::for_each(std::begin(rom.Value), std::end(rom.Value), [&SStr](std::uint8_t val) { SStr << std::setfill('0') << std::setw(2) << std::hex << (int)val; });
	return SStr.str();
};
*/
std::string ToString(const dallas::tID& id)
{
	std::stringstream SStr;
	std::for_each(std::rbegin(id.Value), std::rend(id.Value), [&SStr](std::uint8_t val) { SStr << std::setfill('0') << std::setw(2) << std::hex << (int)val; });
	return SStr.str();
};
/*
std::string ToString(dallas::tCodeError codeError)
{
	switch (codeError)
	{
	case dallas::tCodeError::Success: return "ok";
	case dallas::tCodeError::Connection: return "Key is not connected";
	case dallas::tCodeError::SameKeyID: return "Key ID is the same";
	case dallas::tCodeError::NotWritten: return "Key ID is not written";
	case dallas::tCodeError::WrongKeyID: return "Key ID is wrong";
	}
	return "unknown";
}

enum class tCodeError
{
	Success,
	Connection,
	SameKeyID,
	NotWritten,
	WrongKeyID,
};

void SendRsp(tUDPServer& server, const share_network_udp::tEndpoint& endpoint, boost::property_tree::ptree& pTree, std::stringstream& sstr, const std::string& rsp)
{
	pTree.put("rsp", rsp);
	boost::property_tree::json_parser::write_json(sstr, pTree);
	server.Send(endpoint, sstr.str());
}
*/
void ThreadDallas(std::promise<void>& promise, const std::shared_ptr<dev::tDataSetConfig>& config, const std::shared_ptr<dev::tDataSet>& dataSet)//, tUDPServer& server, const std::shared_ptr<tQueuePack>& queueIn)
{
	try
	{
		const std::string PortID = config->GetDallasPortID();
		if (PortID.empty())
			THROW_INVALID_ARGUMENT("Port ID is empty.");

		boost::asio::io_context ioc;
		dallas::tDALLAS Port(ioc, PortID, dallas::tDALLAS::tSpeed::Norm); // Norm - not all of keys work at the baudrate of 230400
		std::thread Th_ioc([&ioc]() { ioc.run(); });

		std::vector<dallas::tROM> FoundROMs = Port.Search(dallas::tFamilyCode::DS18B20);
		if (FoundROMs.empty())
			THROW_INVALID_ARGUMENT("No sensors connected.");
		std::vector<std::string> ThermoIDs;
		for (auto& rom : FoundROMs)
			ThermoIDs.push_back(ToString(rom.Field.ID));
		config->Load(ThermoIDs);

		std::weak_ptr<dev::tDataSetConfig> ConfigWeak = config;
		std::weak_ptr<dev::tDataSet> DataSetWeak = dataSet;

		while (!DataSetWeak.expired() && !ConfigWeak.expired())
		{
			try
			{
				if (!Port.IsReady())
					THROW_INVALID_ARGUMENT(Port.GetError().message() + " Error Code: " + std::to_string(Port.GetError().value()) + ".");
			}
			catch (...)
			{
				promise.set_exception(std::current_exception());
				break;
			}

			std::vector<dallas::tThermal> ThermoValues = Port.GetTemperature(FoundROMs);
			if (ThermoValues.empty())
				THROW_INVALID_ARGUMENT("Sensors don't respond.");

			auto DataSetConfig = ConfigWeak.lock();
			auto DataSet = DataSetWeak.lock();
			if (!DataSetConfig || !DataSet)
				return;
			dev::tDataSetConfig::tThermoDallasMap SensMap = DataSetConfig->GetThermoDallasMap();
			dev::tDataSet::tThermo DsThermo;
			for (auto& i : ThermoValues)
			{
				std::string SensID = ToString(i.ROM.Field.ID);
				std::string SensLoc = SensMap.contains(SensID) ? SensMap[SensID] : "unknown";
				DsThermo.emplace_back(SensID, SensLoc, std::to_string(dallas::ToDouble(i.Temperature)));
			}
			//DataSet->

			/////////////



			std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // measurements are taken once a second
		}


		/*while (!QueueInWeak.expired())
		{
			try
			{
				if (!Port.IsReady())
					THROW_INVALID_ARGUMENT(Port.GetError().message() + " Error Code: " + std::to_string(Port.GetError().value()) + ".");
			}
			catch (...)
			{
				promise.set_exception(std::current_exception());
				break;
			}

			std::shared_ptr<tQueuePack> QueueIn = QueueInWeak.lock();
			if (!QueueIn->empty())
			{
				tPack Pack = QueueIn->get_front();
				std::stringstream SStr(std::move(Pack.Data));

				boost::property_tree::ptree PTree;
				try
				{
					boost::property_tree::json_parser::read_json(SStr, PTree);
					std::string Cmd = PTree.get<std::string>("cmd");

					if (Cmd == "close")
					{
						SendRsp(server, Pack.Endpoint, PTree, SStr, "ok");
						promise.set_value();
						ioc.stop();
						Th_ioc.join();
						return;
					}
					else if (Cmd == "version")
					{
						PTree.put("version", settings::Version);
						SendRsp(server, Pack.Endpoint, PTree, SStr, "ok");
						continue;
					}

					if (Port.Reset() != dallas::tDALLAS::tStatus::Success)
					{
						SendRsp(server, Pack.Endpoint, PTree, SStr, "not connected");
						continue;
					}

					if (Cmd == "search")
					{
						std::string FamilyCodeStr = PTree.get<std::string>("family_code", "0");
						dallas::tFamilyCode FamilyCode = static_cast<dallas::tFamilyCode>(std::strtoul(FamilyCodeStr.c_str(), nullptr, 16));

						std::vector<dallas::tROM> FoundROMs;
						if (FamilyCode == dallas::tFamilyCode::None)
						{
							FoundROMs = Port.Search();
						}
						else
						{
							FoundROMs = Port.Search(FamilyCode);
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

						SendRsp(server, Pack.Endpoint, PTree, SStr, "ok");
					}
					else if (Cmd == "read_key")
					{
						std::optional<dallas::tID> KeyID = Port.ReadKey();
						if (KeyID.has_value())
						{
							PTree.put("id", ToString(*KeyID));
							SendRsp(server, Pack.Endpoint, PTree, SStr, "ok");
						}
						else
						{
							SendRsp(server, Pack.Endpoint, PTree, SStr, "no key");
						}
					}
					else if (Cmd == "write_key")
					{
						std::string KeyIDStr = PTree.get<std::string>("id", "");
						if (KeyIDStr.size() == 12) // 00000308c4de
						{
							dallas::tID KeyID = ToDallasID(KeyIDStr);
							const dallas::tCodeError CodeError = Port.WriteKey(KeyID);
							SendRsp(server, Pack.Endpoint, PTree, SStr, ToString(CodeError));
						}
						else
						{
							SendRsp(server, Pack.Endpoint, PTree, SStr, "wrong key id");
						}
					}
					else if (Cmd == "read_rom")
					{
						SendRsp(server, Pack.Endpoint, PTree, SStr, "not supported");
					}
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

						if (!ROMs.empty())
						{
							std::vector<dallas::tThermal> Thermo = Port.GetTemperature(ROMs);
							if (!Thermo.empty())
							{
								boost::property_tree::ptree PTreeChildren;
								for (const auto& i : Thermo)
								{
									boost::property_tree::ptree PTreeChild;
									PTreeChild.put("id", ToString(i.ROM.Field.ID));
									PTreeChild.put("temperature", ToDouble(i.Temperature));
									PTreeChildren.push_back(std::make_pair("", PTreeChild));
								}
								PTree.add_child("measurements", PTreeChildren);
								SendRsp(server, Pack.Endpoint, PTree, SStr, "ok");
							}
							else
							{
								SendRsp(server, Pack.Endpoint, PTree, SStr, "no measurements");
							}
						}
						else
						{
							SendRsp(server, Pack.Endpoint, PTree, SStr, "no id");
						}
					}
					else
					{
						SendRsp(server, Pack.Endpoint, PTree, SStr, "unknown");
					}
				}
				catch (...) { } // Format JSON shall be verified in the receiver (e.g. in UDP-Server)
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}*/

		ioc.stop();
		Th_ioc.join();
	}
	catch (...)
	{
		promise.set_exception(std::current_exception());
	}
}
