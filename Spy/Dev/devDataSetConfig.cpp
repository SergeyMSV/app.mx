#include "devDataSetConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tSpyGNSS::tSpyGNSS(boost::property_tree::ptree pTree)
{
	Host = pTree.get<std::string>("gnss.host");
	TargetGLO = pTree.get<std::string>("gnss.target_glo");
	TargetGPS = pTree.get<std::string>("gnss.target_gps");
	Period = pTree.get<uint32_t>("gnss.period");
}

bool tSpyGNSS::IsWrong()
{
	return Host.empty()|| TargetGLO.empty() || TargetGPS.empty() || Period == 0;
}

}

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice)
{
	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_SpyGNSS = config::tSpyGNSS(PTreeConfig);
	m_SpyOutGLO = config::tSpyOutGLO(PTreeConfig);
	m_SpyOutGPS = config::tSpyOutGPS(PTreeConfig);

	boost::property_tree::ptree PTreeDevice;
	boost::property_tree::json_parser::read_json(fileNameDevice, PTreeDevice);
	m_Device = share_config::tDevice(PTreeDevice);
}

}
