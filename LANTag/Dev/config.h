#pragma once

#include <shareConfig.h>

#include <string>

namespace dev
{

class tConfig
{
	share::config::tPlatform m_Platform;
	share::config::tUID m_UID;
	share::config::port::tUDP_Config m_UDPPort;

public:
	explicit tConfig(const std::string& fileNameMX);

	std::string GetPlatformID() const { return m_Platform.ID; }
	std::string GetUID() const { return m_UID.ID; }
	share::config::port::tUDP_Config GetPortUDP() const { return m_UDPPort; }
};

}
