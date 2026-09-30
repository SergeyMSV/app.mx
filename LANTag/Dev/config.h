#pragma once

#include <shareConfig.h>

#include <string>

namespace dev
{

class tDataSetConfig
{
	share::config::tPlatform m_Platform;
	share::config::port::tUDP_Config m_UDPPort;

public:
	explicit tDataSetConfig(const std::string& fileNameMX);

	share::config::tPlatform GetPlatform() const { return m_Platform; }
	share::config::port::tUDP_Config GetPortUDP() const { return m_UDPPort; }
};

}
