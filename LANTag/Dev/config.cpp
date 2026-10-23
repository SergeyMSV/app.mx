#include "config.h"

#include <utilsPath.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tConfig::tConfig()
{
	const std::string mxConfPath = utils::path::GetPathConfigExc("mx").string();

	boost::property_tree::ptree mxPTree;
	boost::property_tree::json_parser::read_json(mxConfPath, mxPTree);
	m_Platform = share::config::tPlatform(mxPTree);
	m_UID = share::config::tUID(mxPTree);
	m_UDPPort = share::config::port::tUDP_Config("network.lantag_port", mxPTree);
}

}
