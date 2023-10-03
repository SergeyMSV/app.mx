#include "main_server.h"

tVectorUInt8 tTWRServer::OnReceived(const tVectorUInt8& data)
{
	m_ReceivedData.insert(m_ReceivedData.end(), data.begin(), data.end());

	tPacketTWRCmd Cmd;
	std::size_t PackSize = tPacketTWRCmd::Find(m_ReceivedData, Cmd);
	if (PackSize || m_ReceivedData.size() > share_network_udp::PacketSizeMax)
		m_ReceivedData.clear();

	return HandlePacket(Cmd);
}

tVectorUInt8 tTWRServer::HandlePacket(tPacketTWRCmd& cmd)
{
	switch (cmd.GetMsgId())
	{
	case TWR::tMsgId::GetVersion: return tPacketTWRRsp::Make(cmd, Version).ToVector();
	case TWR::tMsgId::DEMO_Request:
	case TWR::tMsgId::SPI_Request:
	case TWR::tMsgId::SPI_GetSettings:
	case TWR::tMsgId::SPI_SetChipControl:
	{
		if (PutInQueue(cmd))
			return {};
	}
	}
	return tPacketTWRRsp::Make_ERR(cmd, TWR::tMsgStatus::NotSupported).ToVector();
}

void tTWRServer::OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred)
{
	//std::cout << "HandleSend: " << packet->size() << " cerr: " << error << " --- bytes: " << bytes_transferred << '\n';
}

bool tTWRServer::PutInQueue(const tPacketTWRCmd& cmd)
{
	switch (cmd.GetEndpoint())
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
