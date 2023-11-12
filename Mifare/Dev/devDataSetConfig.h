#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tLog : public share_config::tOutFileCap
{
	tLog() = default;
	explicit tLog(const boost::property_tree::ptree& pTree)
		:share_config::tOutFileCap("log_read", pTree)
	{}
};

}

class tDataSetConfig
{
	share_config::tSPIPort m_SPI;
	share_config::tGPIOPort m_SPI_RST;
	share_config::tPipe m_Pipe;
	config::tLog m_Log;

public:
	tDataSetConfig() = default;
	explicit tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameMX);

	share_config::tSPIPort GetSPI() const { return m_SPI; }
	share_config::tGPIOPort GetSPI_RST() const { return m_SPI_RST; }
	share_config::tPipe GetPipe() { return m_Pipe; }
	config::tLog GetLog() const { return m_Log; }
};

}