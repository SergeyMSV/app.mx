#pragma once

#include "main.h"
#include <vector>

class tTWRServer : public share::network::udp::tUDPServerAsync
{
	tVectorUInt8 m_ReceivedData;

public:
	tTWRServer(boost::asio::io_context& ioc, std::uint16_t port)
		:tUDPServerAsync(ioc, port)
	{}

	void OnReceived(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& data) override;

private:
	void OnSent(boost::shared_ptr<std::vector<std::uint8_t>> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override { }

	void HandlePacket(const tPacketTWRCmdEp& cmd);
	bool PutInQueue(const tPacketTWRCmdEp& cmd);
};
