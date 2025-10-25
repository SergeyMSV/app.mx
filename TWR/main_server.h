#pragma once

#include "main.h"

class tTWRServer : public share::network::udp::tUDPServerAsync
{
	tVectorUInt8 m_ReceivedData;

public:
	tTWRServer(boost::asio::io_context& ioc, std::uint16_t port)
		:tUDPServerAsync(ioc, port)
	{}

	void OnReceived(const share::network::udp::tEndpoint& endpoint, const utils::tVectorUInt8& data) override;

private:
	void OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override { }

	void HandlePacket(tPacketTWRCmdEp& cmd);
	bool PutInQueue(const tPacketTWRCmdEp& cmd);
};
