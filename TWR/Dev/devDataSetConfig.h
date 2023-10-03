#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

namespace dev
{

class tDataSetConfig
{
	share_config::tPlatform m_Platform;
	share_config::tUDPPort m_UDPPort;
	share_config::tSPIPort m_SPI0_CS0;
	share_config::tGPIOPort m_SPI0_CS0_RST;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX);

	share_config::tUDPPort GetUDPPort() const { return m_UDPPort; }
	share_config::tSPIPort GetSPI0_CS0() const { return m_SPI0_CS0; }
	share_config::tGPIOPort GetSPI0_CS0_RST() const { return m_SPI0_CS0_RST; }
};

}
