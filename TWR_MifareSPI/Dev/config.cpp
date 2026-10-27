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

	m_Pipe = share::config::tPipe(appName, mxPTree);

	const std::string SPI_ID = mxPTree.get<std::string>("port.spi." + appName + ".id", "");
	m_SPI_RST = share::config::port::tGPIO_Config("port.spi." + appName + ".reset", mxPTree);
	m_SPI = share::config::port::tSPI_Config(SPI_ID, "port.spi", mxPTree);

	m_Log = share::config::tOutFileCap("log", mxPTree, appName);
}

}
