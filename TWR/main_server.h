#pragma once

#include "main.h"

class tTWRServer : public share_network_udp::tUDPServerAsync
{
	tVectorUInt8 m_ReceivedData;

public:
	tTWRServer(boost::asio::io_context& ioc, std::uint16_t port)
		:tUDPServerAsync(ioc, port)
	{}

	tVectorUInt8 OnReceived(const tVectorUInt8& data) override;

	tVectorUInt8 HandlePacket(tPacketTWRCmd& cmd);

	void OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override;

private:
	bool PutInQueue(const tPacketTWRCmd& cmd);
};
