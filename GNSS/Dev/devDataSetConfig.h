#pragma once

#include <devConfig.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

class tDataSetConfig
{
	share::config::tPlatform m_Platform;
	share::config::tOutFile m_OutFile;
	share::config::port::tUART_Config m_UART;

public:
	tDataSetConfig() = default;
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNamePrivate);

	share::config::tOutFile GetOutGNSS() const { return m_OutFile; }
	share::config::port::tUART_Config GetUART() const { return  m_UART; }
};

}
