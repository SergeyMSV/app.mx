#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tEmail
{
	std::string User;
	std::string Password;
	std::string Host;
	uint16_t Port = 0;
	std::string From;
	bool TLS = false;
	bool Auth = false;

	tEmail() = default;
	explicit tEmail(const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tConfigFiles
{
	std::string msmtprc;
	std::string muttrc;

	tConfigFiles() = default;
	explicit tConfigFiles(const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tFstab
{
	std::string fs;
	std::string size;

	tFstab() = default;
	explicit tFstab(const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

}

class tDataSetConfig
{
	share_config::tDevice m_Device;
	share_config::tUpdateServer m_UpdateServer;
	std::string m_UpdatePath;
	config::tEmail m_Email;
	config::tConfigFiles m_ConfigFiles;
	config::tFstab m_Fstab;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice, const std::string& fileNamePrivate);

	share_config::tDevice GetDevice() const { return m_Device; }
	share_config::tUpdateServer GetUpdateServer() const { return m_UpdateServer; }
	std::string GetUpdatePath() const { return m_UpdatePath; }
	config::tEmail GetEmail() const { return m_Email; }
	config::tConfigFiles GetConfigFiles() const { return m_ConfigFiles; }
	config::tFstab GetFstab() const { return m_Fstab; }
};

}