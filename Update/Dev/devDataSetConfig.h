#pragma once

#include <utilsBase.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tDevice
{
	std::string Type;
	utils::tVersion Version;

	tDevice() = default;
	explicit tDevice(boost::property_tree::ptree pTree);
};

struct tUpdateServer
{
	std::string Host;
	std::string Target;
	std::string TargetList;

	tUpdateServer() = default;
	explicit tUpdateServer(boost::property_tree::ptree pTree);
};

}

class tDataSetConfig
{
	config::tDevice m_Device;
	config::tUpdateServer m_UpdateServer;
	std::string m_UpdatePath;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice);

	config::tDevice GetDevice() const { return m_Device; }
	config::tUpdateServer GetUpdateServer() const { return m_UpdateServer; }
	std::string GetUpdatePath() const { return m_UpdatePath; }
};

}