#pragma once

#include "main.h"

#include <shareNetwork.h>

#include <memory>
#include <string>

class tUDPServer : public share_network_udp::tUDPServerAsync
{
	std::weak_ptr<tQueuePack> m_QueueInWeak;

public:
	tUDPServer(boost::asio::io_context& ioc, std::uint16_t port, const std::shared_ptr<tQueuePack>& queueIn)
		:tUDPServerAsync(ioc, port), m_QueueInWeak(queueIn)
	{}

	void Send(const share_network_udp::tEndpoint& endpoint, const std::string& packet);

private:
	void OnReceived(const share_network_udp::tEndpoint& endpoint, const tVectorUInt8& data) override;
	void OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override { }
};
