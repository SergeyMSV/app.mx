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
	share::config::port::tUART_Config m_Dallas;
	share::config::port::tUART_Config m_UART[3];

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX);

	share::config::port::tUDP_Config GetUDPPort() const { return m_UDPPort; }
	share::config::port::tSPI_Config GetSPI0_CS0() const { return m_SPI0_CS0; }
	share::config::port::tGPIO_Config GetSPI0_CS0_RST() const { return m_SPI0_CS0_RST; }
	share::config::port::tUART_Config GetDallas() const { return m_Dallas; }
	share::config::port::tUART_Config GetUART(std::uint32_t portIndex) const
	{
		if (portIndex > 2)
			return {};
		return m_UART[portIndex];
	}
};

}
