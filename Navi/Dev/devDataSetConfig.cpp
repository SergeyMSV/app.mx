#include "devDataSetConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tEmail::tEmail(const boost::property_tree::ptree& pTree)
{
	To = pTree.get<std::string>("mxnavi.email.to");
	Period = pTree.get<uint32_t>("mxnavi.email.period");
}

bool tEmail::IsWrong() const
{
	return To.empty() || Period == 0; //[TBD] check if email is correct
}

}

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice, const std::string& fileNamePrivate)
{
	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_GNSS = config::tGNSS(PTreeConfig);
	m_Picture = config::tPicture(PTreeConfig);
	m_SpyOutGLO = config::tSpyOutGLO(PTreeConfig);
	m_SpyOutGPS = config::tSpyOutGPS(PTreeConfig);
	m_Log = config::tLog(PTreeConfig);

	boost::property_tree::ptree PTreeDevice;
	boost::property_tree::json_parser::read_json(fileNameDevice, PTreeDevice);
	m_Device = share_config::tDevice(PTreeDevice);

	boost::property_tree::ptree PTreePrivate;
	boost::property_tree::json_parser::read_json(fileNamePrivate, PTreePrivate);
	m_Email = config::tEmail(PTreePrivate);
}

}
