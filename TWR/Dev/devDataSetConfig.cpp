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
	const std::string SPI0_CS0_ID = PTreeMX.get<std::string>("spi0cs0.id", "");
	m_SPI0_CS0_RST = share_config::tGPIOPort("spi0cs0.reset", PTreeMX);

	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_UDPServer = config::tUDPServer(PTreeConfig);
	m_SPI0_CS0 = share_config::tSPIPort(SPI0_CS0_ID, "spi0cs0", PTreeConfig);
}

}
