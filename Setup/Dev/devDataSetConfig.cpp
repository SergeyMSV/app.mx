#include "devDataSetConfig.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tEmail::tEmail(const boost::property_tree::ptree& pTree)
{
	User = pTree.get<std::string>("mxsetup.email.user");
	Password = pTree.get<std::string>("mxsetup.email.password");
	Host = pTree.get<std::string>("mxsetup.email.host");
	Port = pTree.get<uint16_t>("mxsetup.email.port");
	From = pTree.get<std::string>("mxsetup.email.from");
	TLS = pTree.get<bool>("mxsetup.email.tls");
	Auth = pTree.get<bool>("mxsetup.email.auth");
}

bool tEmail::IsWrong() const
{
	return User.empty() || Password.empty() || Host.empty() || Port  == 0 || From.empty(); //[TBD] check if email is correct
}

tConfigFiles::tConfigFiles(const boost::property_tree::ptree& pTree)
{
	std::string pathMsmtprc = pTree.get<std::string>("config_files.msmtprc");
	std::string pathMuttrc = pTree.get<std::string>("config_files.muttrc");

	msmtprc = utils::path::GetPathNormal(pathMsmtprc).string();
	muttrc = utils::path::GetPathNormal(pathMuttrc).string();
}

bool tConfigFiles::IsWrong() const
{
	return msmtprc.empty() || muttrc.empty();
}

tFstab::tFstab(const boost::property_tree::ptree& pTree)
{
	fs = pTree.get<std::string>("fstab.fs");
	size = pTree.get<std::string>("fstab.size");
}

bool tFstab::IsWrong() const
{
	return fs.empty() || size.empty();
}

}

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice, const std::string& fileNamePrivate)
{
	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_UpdateServer = share_config::tUpdateServer(PTreeConfig);
	std::string PathRaw = PTreeConfig.get<std::string>("path");
	m_UpdatePath = utils::path::GetPathNormal(PathRaw).string();
	m_ConfigFiles = config::tConfigFiles(PTreeConfig);
	m_Fstab = config::tFstab(PTreeConfig);

	// If the file is not found it means that nothing has been installed yet.
	if (fileNameDevice.empty())
	{
		m_Device = share_config::tDevice("MXSetup", utils::tVersion());
	}
	else
	{
		boost::property_tree::ptree PTreeDevice;
		boost::property_tree::json_parser::read_json(fileNameDevice, PTreeDevice);
		m_Device = share_config::tDevice(PTreeDevice);
	}

	boost::property_tree::ptree PTreePrivate;
	boost::property_tree::json_parser::read_json(fileNamePrivate, PTreePrivate);
	m_Email = config::tEmail(PTreePrivate);
}

}
