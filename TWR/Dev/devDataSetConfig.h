#pragma once

#include <devConfig.h>

#include <shareConfig.h>

#include <string>

namespace dev
{

class tDataSetConfig
{
	share::config::tPlatform m_Platform;
	share::config::port::tUDP_Config m_UDPPort;
	share::config::port::tSPI_Config m_SPI0_CS0;
	share::config::port::tGPIO_Config m_SPI0_CS0_RST;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX);

	share::config::port::tUDP_Config GetUDPPort() const { return m_UDPPort; }
	share::config::port::tSPI_Config GetSPI0_CS0() const { return m_SPI0_CS0; }
	share::config::port::tGPIO_Config GetSPI0_CS0_RST() const { return m_SPI0_CS0_RST; }
};

}
