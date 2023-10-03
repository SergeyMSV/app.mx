#include "devDataSetConfig.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tUDPServer::tUDPServer(const boost::property_tree::ptree& pTree)
{
	Port = pTree.get<std::uint16_t>("server.port", 0);
}

bool tUDPServer::IsWrong() const
{
	return Port == 0;
}

}

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(fileNameMX, PTreeMX);
	m_Platform = share_config::tPlatform(PTreeMX);

	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_UDPServer = config::tUDPServer(PTreeConfig);
	m_SPI_0_0 = share_config::tSPIPort("spi-0-0", m_Platform.ID, PTreeConfig);
	m_SPI_0_0_RST = share_config::tGPIOPort("spi-0-0-rst", m_Platform.ID, PTreeConfig);
}

}
