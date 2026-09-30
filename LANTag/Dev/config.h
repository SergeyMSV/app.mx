#pragma once

#include <shareConfig.h>

#include <string>

namespace dev
{

class tConfig
{
	share::config::tPlatform m_Platform;
	share::config::port::tUDP_Config m_UDPPort;

public:
	explicit tConfig(const std::string& fileNameMX);

	share::config::tPlatform GetPlatform() const { return m_Platform; }
	share::config::port::tUDP_Config GetPortUDP() const { return m_UDPPort; }
};

}
