#pragma once

#include <shareConfig.h>

#include <string>

namespace dev
{

class tConfig
{
	share::config::port::tSPI_Config m_SPI;
	share::config::port::tGPIO_Config m_SPI_RST;
	share::config::tPipe m_Pipe;
	share::config::tOutFileCap m_Log;

public:
	tConfig() = default;
	explicit tConfig(const std::string& fileNameConfig, const std::string& fileNameMX);

	share::config::port::tSPI_Config GetSPI() const { return m_SPI; }
	share::config::port::tGPIO_Config GetSPI_RST() const { return m_SPI_RST; }
	share::config::tPipe GetPipe() { return m_Pipe; }
	share::config::tOutFileCap GetLog() const { return m_Log; }
};

}