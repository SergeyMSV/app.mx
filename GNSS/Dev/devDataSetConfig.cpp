#include "devDataSetConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNamePrivate)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTree);

	boost::property_tree::ptree PTreePrivate;
	boost::property_tree::json_parser::read_json(fileNamePrivate, PTreePrivate);
	m_Platform = share::config::tPlatform(PTreePrivate);

	m_OutFile = share::config::tOutFile("out", PTree);
	m_UART = share::config::port::tUART_Config("uart", m_Platform.ID, PTree);
}

}
