#include "devDataSetConfig.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice)
{
	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_UpdateServer = share_config::tUpdateServer(PTreeConfig);
	std::string PathRaw = PTreeConfig.get<std::string>("path");
	m_UpdatePath = utils::path::GetPathNormal(PathRaw).string();

	boost::property_tree::ptree PTreeDevice;
	boost::property_tree::json_parser::read_json(fileNameDevice, PTreeDevice);
	m_Device = share_config::tDevice(PTreeDevice);
}

}
