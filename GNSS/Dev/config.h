#pragma once

#include <devConfig.h>

#include <utilsPath.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

class tConfig
{
	share::config::tPlatform m_Platform;
	share::config::port::tUART_Config m_UART;
	share::config::tOutFile m_OutFile;
	share::config::tOutFileCap m_Log;

public:
	explicit tConfig(const std::filesystem::path& a_path);

	share::config::tOutFile GetOutGNSS() const { return m_OutFile; }
	share::config::port::tUART_Config GetUART() const { return  m_UART; }
	share::config::tOutFileCap GetLog() const { return m_Log; }
};

}
