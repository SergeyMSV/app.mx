#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tUDPServer
{
	std::uint16_t Port;

	tUDPServer() = default;
	explicit tUDPServer(const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

}

class tDataSetConfig
{
	share_config::tPlatform m_Platform;
	config::tUDPServer m_UDPServer;
	share_config::tSPIPort m_SPI_0_0;
	share_config::tGPIOPort m_SPI_0_0_RST;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX);

	config::tUDPServer GetUDPServer() const { return m_UDPServer; }
	share_config::tSPIPort GetSPI_0_0() const { return m_SPI_0_0; }
	share_config::tGPIOPort GetSPI_0_0_RST() const { return m_SPI_0_0_RST; }
};

}