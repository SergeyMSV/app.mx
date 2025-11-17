#pragma once

#include "main.h"

#include <shareNetwork.h>

#include <devDataSetConfig.h>

#include <memory>

class tLANTagServer : public share::network::udp::tUDPServerAsync
{
	std::weak_ptr<dev::tDataSetConfig> m_DataSetConfig;

public:
	tLANTagServer(boost::asio::io_context& ioc, std::uint16_t port, const std::shared_ptr<dev::tDataSetConfig>& dataSetConfig)
		:tUDPServerAsync(ioc, port), m_DataSetConfig(dataSetConfig)
	{}

	void OnReceived(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& data) override;

private:
	void OnSent(boost::shared_ptr<std::vector<std::uint8_t>> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override;
};
