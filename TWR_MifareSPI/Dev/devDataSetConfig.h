#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tLog : public share::config::tOutFileCap
{
	tLog() = default;
	explicit tLog(const boost::property_tree::ptree& pTree)
		:share::config::tOutFileCap("log_read", pTree)
	{}
};

}

class tDataSetConfig
{
	share::config::port::tSPI_Config m_SPI;
	share::config::port::tGPIO_Config m_SPI_RST;
	share::config::tPipe m_Pipe;
	config::tLog m_Log;

public:
	tDataSetConfig() = default;
	explicit tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX);

	share::config::port::tSPI_Config GetSPI() const { return m_SPI; }
	share::config::port::tGPIO_Config GetSPI_RST() const { return m_SPI_RST; }
	share::config::tPipe GetPipe() { return m_Pipe; }
	config::tLog GetLog() const { return m_Log; }
};

}