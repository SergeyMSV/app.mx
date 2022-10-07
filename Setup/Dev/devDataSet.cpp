#include "devDataSet.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tEmail::tEmail(boost::property_tree::ptree a_PTree)
{
	User = a_PTree.get<std::string>("email.user");
	Password = a_PTree.get<std::string>("email.password");
	Host = a_PTree.get<std::string>("email.host");
	Port = a_PTree.get<uint16_t>("email.port");
	From = a_PTree.get<std::string>("email.from");
	TLS = a_PTree.get<bool>("email.tls");
	Auth = a_PTree.get<bool>("email.auth");
}

tConfigFiles::tConfigFiles(boost::property_tree::ptree a_PTree)
{
	std::string pathMsmtprc = a_PTree.get<std::string>("config_files.msmtprc");
	std::string pathMuttrc = a_PTree.get<std::string>("config_files.muttrc");

	msmtprc = utils::linux::GetPath(pathMsmtprc);
	muttrc = utils::linux::GetPath(pathMuttrc);
}

}

tDataSetConfig::tDataSetConfig(const std::string& a_fileName)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(a_fileName, PTree);

	m_Email = config::tEmail(PTree);
	m_ConfigFiles = config::tConfigFiles(PTree);
}

}
