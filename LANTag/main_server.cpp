#include "main_server.h"
#include <utilsLinux.h>

#include <chrono>
#include <sstream>
#include <thread>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

void tLANTagServer::OnReceived(const share_network_udp::tEndpoint& endpoint, const tVectorUInt8& data)
{
	if (data.size() > 64) // [#] max. size of an incoming data
		return;

	auto SendRsp = [&](boost::property_tree::ptree& pTree, std::stringstream& sstr, const std::string& rsp)
		{
			pTree.put("rsp", rsp);
			boost::property_tree::json_parser::write_json(sstr, pTree);
			std::string RspStr = sstr.str();
			tVectorUInt8 Data(RspStr.begin(), RspStr.end());
			Data.push_back(0);
			Send(endpoint, Data);
		};

	std::stringstream SStr(std::string(data.begin(), data.end()));
	boost::property_tree::ptree PTree;
	try
	{
		boost::property_tree::json_parser::read_json(SStr, PTree);
		std::string Cmd = PTree.get<std::string>("cmd");

		if (Cmd == "get_tag")
		{
			std::shared_ptr<dev::tDataSetConfig> DataSetConfig = m_DataSetConfig.lock();
			if (!DataSetConfig)
				return;
			PTree.put("platform_id", DataSetConfig->GetPlatform().ID);
			PTree.put("hostname", utils::linux::CmdLine("hostname"));
			PTree.put("uptime", utils::linux::GetUptimeString());
			SendRsp(PTree, SStr, "ok");
		}
		else if (Cmd == "reboot")
		{
			SendRsp(PTree, SStr, "ok");
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			utils::linux::CmdLine("reboot");
		}
		else if (Cmd == "halt")
		{
			SendRsp(PTree, SStr, "ok");
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			utils::linux::CmdLine("halt");
		}
		//else // Send nothing in response to unknown request.
		//{
		//	SendRsp(PTree, SStr, "unknown");
		//}
	}
	catch (...) {} // Format JSON shall be verified in the receiver (e.g. in UDP-Server)
}

void tLANTagServer::OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	//std::cout << "HandleSend: " << packet->size() << " cerr: " << error << " --- bytes: " << bytes_transferred << '\n';
}