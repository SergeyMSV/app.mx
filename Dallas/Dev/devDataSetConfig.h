#pragma once

#include <shareConfig.h>

#include <string>

namespace dev
{

class tDataSetConfig
{
	share_config::tUDPPort m_UDPPort;
	std::string m_DallasPortID;

public:
	tDataSetConfig(std::uint16_t udpPort, const std::string& dallasPortID);
	explicit tDataSetConfig(const std::string& fileNameMX);

	share_config::tUDPPort GetUDPPort() const { return m_UDPPort; }
	std::string GetDallasPortID() const { return m_DallasPortID; }
};

}
