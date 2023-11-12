#include "devDataSetConfig.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(fileNameMX, PTreeMX);
	m_Platform = share_config::tPlatform(PTreeMX);
	m_Pipe = share_config::tPipe("card_mifare", m_Platform.ID, PTreeMX);

	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_SPI = share_config::tSPIPort("spi", m_Platform.ID, PTreeConfig);
	m_SPI_RST = share_config::tGPIOPort("spi-rst", m_Platform.ID, PTreeConfig);
	m_Log = config::tLog(PTreeConfig);
}

}
