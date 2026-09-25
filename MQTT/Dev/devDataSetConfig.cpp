#include "devDataSetConfig.h"

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(const std::string& a_filename_mx)
{
	boost::property_tree::ptree PTreeMX;
	boost::property_tree::json_parser::read_json(a_filename_mx, PTreeMX);
	m_Platform = share::config::tPlatform(PTreeMX);
	m_UID = share::config::tUID(PTreeMX);
	m_Family = share::config::tFamily(PTreeMX);
}

}
