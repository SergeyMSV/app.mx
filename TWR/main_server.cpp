#include "main_server.h"

void tTWRServer::OnReceived(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& data)
{
	m_ReceivedData.insert(m_ReceivedData.end(), data.begin(), data.end());

	tPacketTWRCmdEp Cmd;
	std::size_t PackSize = tTWRPacketCmd::Find(m_ReceivedData, Cmd.Value);
	if (PackSize || m_ReceivedData.size() > share::network::udp::PacketSizeMax)
		m_ReceivedData.clear();
	Cmd.Endpoint = endpoint;

	HandlePacket(Cmd);
}

void tTWRServer::HandlePacket(const tPacketTWRCmdEp& cmd)
{
	std::vector<std::uint8_t> Pack;

	switch (cmd.Value.GetMsgId())
	{
	case tTWRMsgId::GetVersion: Pack = tTWRPacketRsp::Make(cmd.Value, settings::Version).ToVector();
		[[fallthrough]];
	case tTWRMsgId::DEMO_Request:
	case tTWRMsgId::SPI_Request:
	case tTWRMsgId::SPI_GetSettings:
	case tTWRMsgId::SPI_SetChipControl:
	{
		if (PutInQueue(cmd))
			return;
	}
	}

	Pack = tTWRPacketRsp::Make_ERR(cmd.Value, tTWRMsgStatus::NotSupported).ToVector();
	Send(cmd.Endpoint, Pack);
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
