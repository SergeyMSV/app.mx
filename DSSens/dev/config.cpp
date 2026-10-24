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
	m_Dallas = share::config::port::tUART_Config("port.uart_" + appName, m_Platform.ID, mxPTree);

	for (auto& i : mxPTree.get_child("sensor"))
	{
		const boost::property_tree::ptree& obj = i.second;
		if (obj.empty())
			continue;
		auto it = obj.begin();
		m_SensorsProp.emplace_back(it->first, it->second.data());
	}

	m_OutFile = share::config::tOutFile("out", mxPTree, appName);
}

config::tSensorProp tConfig::GetSensorProp(std::string a_romID) const
{
	a_romID.resize(4);
	std::transform(a_romID.begin(), a_romID.end(), a_romID.begin(), [](unsigned char c) { return std::toupper(c); });
	std::string sensID = "DS18B20_" + a_romID;
	for (const auto& i : m_SensorsProp)
	{
		if (i.ID == sensID)
			return i;
	}
	return {};
}

}
