#include "config.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tConfig::tConfig()
{
	const std::string mxConfPath = utils::path::GetPathConfigExc("mx").string();

	boost::property_tree::ptree mxPTree;
	boost::property_tree::json_parser::read_json(mxConfPath, mxPTree);

	m_Platform = share::config::tPlatform(mxPTree);
	m_UDPPort = share::config::port::tUDP_Config("network.twr_port", mxPTree);
	const std::string SPI0_CS0_ID = mxPTree.get<std::string>("port.spi.twr.id", "");
	m_SPI0_CS0_RST = share::config::port::tGPIO_Config("port.spi.twr.reset", mxPTree);
	m_SPI0_CS0 = share::config::port::tSPI_Config(SPI0_CS0_ID, "port.spi", mxPTree);
	m_Dallas = share::config::port::tUART_Config("port.uart_dallas", m_Platform.ID, mxPTree);
	m_UART[0] = share::config::port::tUART_Config("port.uart_0", m_Platform.ID, mxPTree);
	m_UART[1] = share::config::port::tUART_Config("port.uart_1", m_Platform.ID, mxPTree);
	m_UART[2] = share::config::port::tUART_Config("port.uart_2", m_Platform.ID, mxPTree);
	m_UART[3] = share::config::port::tUART_Config("port.uart_3", m_Platform.ID, mxPTree);
}

}
