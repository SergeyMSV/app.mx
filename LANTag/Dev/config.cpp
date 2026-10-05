#include "config.h"

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tConfig::tConfig(const std::string& fileNameMX)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(fileNameMX, PTreeMX);
	m_Platform = share::config::tPlatform(PTreeMX);
	m_UID = share::config::tUID(PTreeMX);
	m_UDPPort = share::config::port::tUDP_Config("network.lantag_port", PTreeMX);
}

}
