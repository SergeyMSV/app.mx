#include "config.h"

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tConfig::tConfig(const std::filesystem::path& a_path)
{
	const std::string appName = utils::path::GetAppNameMain(a_path);
	const std::string mxConfPath = utils::path::GetPathConfigExc("mx").string();

	boost::property_tree::ptree mxPTree;
	boost::property_tree::json_parser::read_json(mxConfPath, mxPTree);

	m_Platform = share::config::tPlatform(mxPTree);
	m_UART = share::config::port::tUART_Config("port.uart_" + appName, m_Platform.ID, mxPTree);

	m_OutFile = share::config::tOutFile("out", mxPTree, appName);
	m_Log = share::config::tOutFileCap("log", mxPTree, appName);
}

}
