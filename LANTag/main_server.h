#pragma once
#include "main.h"

#include <shareNetwork.h>

#include <devDataSetConfig.h>

#include <memory>

class tLANTagServer : public share_network_udp::UDP_Server
{
	std::weak_ptr<dev::tDataSetConfig> m_DataSetConfig;

public:
	tLANTagServer(boost::asio::io_context& ioc, std::uint16_t port, const std::shared_ptr<dev::tDataSetConfig>& dataSetConfig)
		:UDP_Server(ioc, port), m_DataSetConfig(dataSetConfig)
	{}

	tVectorUInt8 OnReceived(const tVectorUInt8& data) override;

private:
	tVectorUInt8 HandlePacket(const tVectorUInt8& cmd);

	void OnSent(boost::shared_ptr<tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override;
};
