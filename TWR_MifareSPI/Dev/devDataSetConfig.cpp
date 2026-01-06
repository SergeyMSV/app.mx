#include "devDataSetConfig.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(fileNameMX, PTreeMX);
	m_Pipe = share::config::tPipe("mifare", PTreeMX);
	const std::string SPI_ID = PTreeMX.get<std::string>("spi_mifare.id", "");
	m_SPI_RST = share::config::port::tGPIO_Config("spi_mifare.reset", PTreeMX);

	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_SPI = share::config::port::tSPI_Config(SPI_ID, "spi_mifare", PTreeConfig);
	m_Log = config::tLog(PTreeConfig);
}

}
