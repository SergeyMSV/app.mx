#include "devDataSetConfig.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(std::uint16_t udpPort, const std::string& dallasPortID)
	:m_UDPPort(udpPort), m_DallasPortID(dallasPortID)
{
}

tDataSetConfig::tDataSetConfig(const std::string& fileNameMX)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(fileNameMX, PTreeMX);
	m_UDPPort = share_config::tUDPPort("network.dallas_port", PTreeMX);
	m_DallasPortID = PTreeMX.get<std::string>("uart.dallas.id", "");
}

}
