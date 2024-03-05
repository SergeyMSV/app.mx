#include "devDataSetConfig.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(const std::string& fileNameMX)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(fileNameMX, PTreeMX);
	m_Platform = share_config::tPlatform(PTreeMX);
	m_UDPPort = share_config::tUDPPort("network.lantag_port", PTreeMX);
}

}
