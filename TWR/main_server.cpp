#include "main_server.h"

void tTWRServer::OnReceived(const share_network_udp::tEndpoint& endpoint, const utils::tVectorUInt8& data)
{
	m_ReceivedData.insert(m_ReceivedData.end(), data.begin(), data.end());

	tPacketTWRCmdEp Cmd;
	std::size_t PackSize = tPacketTWRCmd::Find(m_ReceivedData, Cmd.Value);
	if (PackSize || m_ReceivedData.size() > share_network_udp::PacketSizeMax)
		m_ReceivedData.clear();
	Cmd.Endpoint = endpoint;

	HandlePacket(Cmd);
}

void tTWRServer::HandlePacket(tPacketTWRCmdEp& cmd)
{
	utils::tVectorUInt8 Pack;

	switch (cmd.Value.GetMsgId())
	{
	case TWR::tMsgId::GetVersion: Pack = tPacketTWRRsp::Make(cmd.Value, settings::Version).ToVector();
	case TWR::tMsgId::DEMO_Request:
	case TWR::tMsgId::SPI_Request:
	case TWR::tMsgId::SPI_GetSettings:
	case TWR::tMsgId::SPI_SetChipControl:
	{
		if (PutInQueue(cmd))
			return;
	}
	}

	Pack = tPacketTWRRsp::Make_ERR(cmd.Value, TWR::tMsgStatus::NotSupported).ToVector();
	Send(cmd.Endpoint, Pack);
}

bool tTWRServer::PutInQueue(const tPacketTWRCmdEp& cmd)
{
	switch (cmd.Value.GetEndpoint())
	{
	case TWR::tEndpoint::DEMO:
	{
		TWRQueue.DEMO.push_back(cmd);
		return true;
	}
	case TWR::tEndpoint::SPI0_CS0:
	{
		TWRQueue.SPI0_CS0.push_back(cmd);
		return true;
	}
	//case TWR::tEndpoint::SPI0_CS1:
	//case TWR::tEndpoint::SPI0_CS2:
	}
	return false;
}
