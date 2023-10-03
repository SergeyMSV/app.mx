#include "devDataSetConfig.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(fileNameMX, PTreeMX);
	m_Platform = share_config::tPlatform(PTreeMX);
	m_UDPPort = share_config::tUDPPort("network.twr_port", PTreeMX);
	const std::string SPI0_CS0_ID = PTreeMX.get<std::string>("spi.twr.id", "");
	m_SPI0_CS0_RST = share_config::tGPIOPort("spi.twr.reset", PTreeMX);

	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_SPI0_CS0 = share_config::tSPIPort(SPI0_CS0_ID, "spi", PTreeConfig);
}

}
