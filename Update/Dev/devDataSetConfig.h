#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

class tDataSetConfig
{
	share_config::tDevice m_Device;
	share_config::tUpdateServer m_UpdateServer;
	std::string m_UpdatePath;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice);

	share_config::tDevice GetDevice() const { return m_Device; }
	share_config::tUpdateServer GetUpdateServer() const { return m_UpdateServer; }
	std::string GetUpdatePath() const { return m_UpdatePath; }
};

}