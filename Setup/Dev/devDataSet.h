#pragma once

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
	explicit tEmail(boost::property_tree::ptree a_PTree);
};

struct tConfigFiles
{
	std::string msmtprc;
	std::string muttrc;

	tConfigFiles() = default;
	explicit tConfigFiles(boost::property_tree::ptree a_PTree);
};

}

class tDataSetConfig
{
	config::tEmail m_Email;
	config::tConfigFiles m_ConfigFiles;

public:
	explicit tDataSetConfig(const std::string& a_fileName);

	config::tEmail GetEmail() const { return m_Email; }
	config::tConfigFiles GetConfigFiles() const { return m_ConfigFiles; }
};

}