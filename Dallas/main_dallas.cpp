#include "main.h"
#include "main_server.h"

#include "devDataSetConfig.h"

#include <cstdlib>
#include <future>
#include <iomanip>
#include <memory>
#include <sstream>

#include <utilsBase.h>
#include <utilsDALLAS.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace dallas = utils::DALLAS;

std::string ToString(const dallas::tROM& rom)
{
	std::stringstream SStr;
	std::for_each(std::begin(rom.Value), std::end(rom.Value), [&SStr](std::uint8_t val) { SStr << std::setfill('0') << std::setw(2) << std::hex << (int)val; });
	return SStr.str();
};

std::string ToString(const dallas::tID& id)
{
	std::stringstream SStr;
	std::for_each(std::rbegin(id.Value), std::rend(id.Value), [&SStr](std::uint8_t val) { SStr << std::setfill('0') << std::setw(2) << std::hex << (int)val; });
	return SStr.str();
};

void SendRsp(tUDPServer& server, const share_network_udp::tEndpoint& endpoint, boost::property_tree::ptree& pTree, std::stringstream& sstr, const std::string& rsp)
{
	pTree.put("rsp", rsp);
	boost::property_tree::json_parser::write_json(sstr, pTree);
	server.Send(endpoint, sstr.str());
}

void ThreadDallas(std::promise<void>& promise, const std::shared_ptr<dev::tDataSetConfig>& config, tUDPServer& server, const std::shared_ptr<tQueuePack>& queueIn)
{
	try
	{
		const std::string PortID = config->GetDallasPortID();
		if (PortID.empty())
			return;

		boost::asio::io_context ioc;
		dallas::tDALLAS Port(ioc, PortID, dallas::tDALLAS::tSpeed::Norm); // Norm - not all of keys work at the baudrate of 230400
		std::thread Th_ioc([&ioc]() { ioc.run(); });

		std::weak_ptr<tQueuePack> QueueInWeak = queueIn;

		while (!QueueInWeak.expired())
		{
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
								PTreeChild.put("", ToString(i));
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
							PTree.put("id", ToString(*KeyID));
						SendRsp(server, Pack.Endpoint, PTree, SStr, "ok");
					}
					else if (Cmd == "write_key")
					{
						std::string KeyIDStr = PTree.get<std::string>("id", "");
						if (KeyIDStr.size() == 12) // 00000308c4de
						{
							auto KeyIDStrIter = KeyIDStr.rbegin();
							dallas::tID KeyID{};
							for (std::size_t i = 0; i < 6; ++i) // sizeof(dallas::tID::Value) = 6
							{
								KeyID.Value[i] = utils::Read<std::uint8_t>(KeyIDStrIter, KeyIDStrIter + 2, utils::tRadix::hex);
								KeyIDStrIter += 2;
							}
							const bool Written = Port.WriteKey(KeyID);
							if (Written)
							{
								std::optional<dallas::tID> KeyIDWritten = Port.ReadKey();
								if (KeyIDWritten.has_value() && *KeyIDWritten == KeyID)
								{
									SendRsp(server, Pack.Endpoint, PTree, SStr, "ok");
									continue;
								}
							}
							SendRsp(server, Pack.Endpoint, PTree, SStr, "key doesn't match");
						}
						else
						{
							SendRsp(server, Pack.Endpoint, PTree, SStr, "wrong key");
						}
					}
					else if (Cmd == "read_rom")
					{
						SendRsp(server, Pack.Endpoint, PTree, SStr, "not supported");
					}
					else if (Cmd == "thermo")
					{
						//for (auto& msg : PTree.get_child("roms"))
						//{
						//	std::cout << "  -> " << msg.second.get_value<std::string>() << '\n';
						//}
						//std::string FamilyCodeStr = PTree.get<std::string>("family_code", "");

						//std::vector<dallas::tROM> Found
						//std::vector<tThermal> Thermo = Port.GetTemperature(Found);

						SendRsp(server, Pack.Endpoint, PTree, SStr, "not supported");
					}
					else
					{
						SendRsp(server, Pack.Endpoint, PTree, SStr, "unknown");
					}
				}
				catch (...) { } // Format JSON shall be verified in the receiver (e.g. in UDP-Server)
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	catch (...)
	{
		promise.set_exception(std::current_exception());
	}
}
