#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

class tDataSetConfig
{
	share_config::tPlatform m_Platform;
	share_config::tUDPPort m_UDPPort;

public:
	explicit tDataSetConfig(const std::string& fileNameMX);

	share_config::tPlatform GetPlatform() const { return m_Platform; }
	share_config::tUDPPort GetPortUDP() const { return m_UDPPort; }
};

}
