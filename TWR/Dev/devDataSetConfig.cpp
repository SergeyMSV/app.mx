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
	m_Platform = share::config::tPlatform(PTreeMX);
	m_UDPPort = share::config::port::tUDP_Config("network.twr_port", PTreeMX);
	const std::string SPI0_CS0_ID = PTreeMX.get<std::string>("spi.twr.id", "");
	m_SPI0_CS0_RST = share::config::port::tGPIO_Config("spi.twr.reset", PTreeMX);

	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_SPI0_CS0 = share::config::port::tSPI_Config(SPI0_CS0_ID, "spi", PTreeConfig);
	m_Dallas = share::config::port::tUART_Config("uart_dallas", m_Platform.ID, PTreeConfig);
	m_UART[0] = share::config::port::tUART_Config("uart_0", m_Platform.ID, PTreeConfig);
	m_UART[1] = share::config::port::tUART_Config("uart_1", m_Platform.ID, PTreeConfig);
	m_UART[2] = share::config::port::tUART_Config("uart_2", m_Platform.ID, PTreeConfig);
	m_UART[3] = share::config::port::tUART_Config("uart_3", m_Platform.ID, PTreeConfig);
}

}
