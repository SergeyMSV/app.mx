#pragma once

#include "main.h"

class tTWRServer : public share_network_udp::UDP_Server
{
	tVectorUInt8 m_ReceivedData;

public:
	tTWRServer(boost::asio::io_context& io_context, std::uint16_t port)
		:UDP_Server(io_context, port)
	{}

	tVectorUInt8 OnReceived(const tVectorUInt8& data) override
	{
		m_ReceivedData.insert(m_ReceivedData.end(), data.begin(), data.end());

		tPacketTWRCmd Cmd;
		std::size_t PackSize = tPacketTWRCmd::Find(m_ReceivedData, Cmd);
		if (PackSize || m_ReceivedData.size() > share_network_udp::PacketSizeMax)
			m_ReceivedData.clear();

		return HandlePacket(Cmd);
	}

	tVectorUInt8 HandlePacket(tPacketTWRCmd& cmd);

	void OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override
	{
		//std::cout << "HandleSend: " << packet->size() << " cerr: " << error << " --- bytes: " << bytes_transferred << '\n';
	}

private:
	bool PutInQueue(const tPacketTWRCmd& cmd);
};
