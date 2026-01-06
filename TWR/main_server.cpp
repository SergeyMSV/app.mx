#include "main_server.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

void tTWRServer::OnReceived(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& data)
{
	// One UDP packet must contain only one full JSON-packet.
	if (HandlePacketJson(endpoint, data))
		return;

	std::vector<std::uint8_t>& ReceivedData = m_ReceivedData[endpoint.port()];

	ReceivedData.insert(ReceivedData.end(), data.begin(), data.end()); // [TBD] it shall collect data from different endpoints separately for each enpoint.

	// The rest is for binary protocol.
	//m_ReceivedData.insert(m_ReceivedData.end(), data.begin(), data.end()); // [TBD] it shall collect data from different endpoints separately for each enpoint.
	
	// data[0] == '{' -> JSON
	// data[0] == '*' -> Star (binary)

	tPacketTWRCmdEp Cmd;
	std::size_t PackSize = tTWRPacketCmd::Find(ReceivedData, Cmd.Value);
	if (PackSize || ReceivedData.size() > share::network::udp::PacketSizeMax)
		ReceivedData.clear();
	Cmd.Endpoint = endpoint;

	HandlePacketBinary(Cmd);
}

void tTWRServer::SendResponse(const share::network::udp::tEndpoint& endpoint, boost::property_tree::ptree& pTree, const std::string& rsp)
{
	pTree.put("rsp", rsp);
	std::stringstream SStr;
	boost::property_tree::json_parser::write_json(SStr, pTree);
	Send(endpoint, SStr.str());
}

void tTWRServer::HandlePacketBinary(const tPacketTWRCmdEp& cmd)
{
	std::vector<std::uint8_t> Pack;

	switch (cmd.Value.GetMsgId())
	{
	case tTWRMsgId::GetVersion: Pack = tTWRPacketRsp::Make(cmd.Value, settings::Version).ToVector(); break;
	default:
	{
		if (PutInQueue(cmd))
			return;
		break;
	}
	}

	Pack = tTWRPacketRsp::Make_ERR(cmd.Value, tTWRMsgStatus::NotSupported).ToVector();
	Send(cmd.Endpoint, Pack);
}

bool tTWRServer::HandlePacketJson(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& cmd)
{
	if (cmd.empty() || cmd[0] != '{')
		return false;

	boost::property_tree::ptree PTree;
	try
	{
		std::string CmdStr(cmd.begin(), cmd.end());
		std::stringstream SStr(CmdStr);
		boost::property_tree::json_parser::read_json(SStr, PTree);

		const std::string Ep = PTree.get<std::string>("ep", "unknown");
		if (Ep == "control")
		{
			std::string Cmd = PTree.get<std::string>("cmd");
			if (Cmd == "version")
			{
				PTree.put("version", settings::Version);
				SendResponse(endpoint, PTree, "ok");
			}
		}
		else if (Ep == "dallas")
		{
			tPacketTWRDALLASCmdEp Cmd{ .Endpoint = endpoint, .Value = std::move(CmdStr) };
			TWRQueue.DALLAS.push_back(std::move(Cmd));
			return true;
		}

		return true; // [TBD] maybe false... maybe not...
	}
	catch (...) {} // Format JSON shall be verified on the receiver's side (e.g. in UDP-Server).

	return false;
}

bool tTWRServer::PutInQueue(const tPacketTWRCmdEp& cmd)
{
	switch (cmd.Value.GetEndpoint())
	{
	case tTWREndpoint::DEMO:
	{
		TWRQueue.DEMO.push_back(cmd);
		return true;
	}
	case tTWREndpoint::SPI0_CS0:
	{
		TWRQueue.SPI0_CS0.push_back(cmd);
		return true;
	}
	//case tTWREndpoint::SPI0_CS1:
	//case tTWREndpoint::SPI0_CS2:
	}
	return false;
}
