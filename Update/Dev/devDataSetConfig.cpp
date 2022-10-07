#include "devDataSetConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tUpdateServer::tUpdateServer(const boost::property_tree::ptree& pTree)
{
	Host = pTree.get<std::string>("server.host");
	Target = pTree.get<std::string>("server.target");
	TargetList = pTree.get<std::string>("server.target_list");
}

}

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice)
{
	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_UpdateServer = config::tUpdateServer(PTreeConfig);
	std::string PathRaw = PTreeConfig.get<std::string>("path");
	m_UpdatePath = utils::linux::GetPath(PathRaw);

	boost::property_tree::ptree PTreeDevice;
	boost::property_tree::json_parser::read_json(fileNameDevice, PTreeDevice);
	m_Device = share_config::tDevice(PTreeDevice);
}

}
