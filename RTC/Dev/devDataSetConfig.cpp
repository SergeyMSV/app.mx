#include "devDataSetConfig.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(const std::string& fileNameMX, const std::string& fileNameMXDev)
	:m_FileNameMXDev(fileNameMXDev)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(fileNameMX, PTreeMX);
	m_Pipe = share_config::tPipe("thermo", PTreeMX);
	m_DallasPortID = PTreeMX.get<std::string>("uart.dallas.id", "");
}

void tDataSetConfig::Load(const std::vector<std::string>& thermoIDs)
{
	if (thermoIDs.empty())
		return;
	boost::property_tree::ptree PTreeMXDev;
	boost::property_tree::json_parser::read_json(m_FileNameMXDev, PTreeMXDev);
	boost::property_tree::ptree Dev = PTreeMXDev.get_child("dallas_thermo");
	for (auto& id : thermoIDs)
	{
		m_ThermoDallasMap[id] = Dev.get<std::string>(id, "");
	}
}

}
