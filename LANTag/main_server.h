#pragma once
#include "main.h"

#include <shareNetwork.h>

#include <devDataSetConfig.h>

#include <memory>

class tLANTagServer : public share_network_udp::tUDPServerAsync
{
	std::weak_ptr<dev::tDataSetConfig> m_DataSetConfig;

public:
	tLANTagServer(boost::asio::io_context& ioc, std::uint16_t port, const std::shared_ptr<dev::tDataSetConfig>& dataSetConfig)
		:tUDPServerAsync(ioc, port), m_DataSetConfig(dataSetConfig)
	{}

	void OnReceived(const share_network_udp::tEndpoint& endpoint, const tVectorUInt8& data) override;

private:
	tVectorUInt8 HandlePacket(const share_network_udp::tEndpoint& endpoint, const tVectorUInt8& cmd);

	void OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override;
};
