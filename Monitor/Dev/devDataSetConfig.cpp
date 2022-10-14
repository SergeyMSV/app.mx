#include "devDataSetConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tEmail::tEmail(const boost::property_tree::ptree& pTree)
{
	To = pTree.get<std::string>("mxmonitor.email.to");
	PeriodMinimum = pTree.get<uint32_t>("mxmonitor.email.period_minimum");
}

bool tEmail::IsWrong() const
{
	return To.empty() || PeriodMinimum == 0; //[TBD] check if email is correct
}

tBoardRAM::tBoardRAM(const boost::property_tree::ptree& pTree)
{
	FreeDiff = pTree.get<uint32_t>("ram.free_diff");
	UsedDiff = pTree.get<uint32_t>("ram.used_diff");
	Period = pTree.get<uint32_t>("ram.period");
	Qty = pTree.get<uint32_t>("ram.qty");
}

}

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice, const std::string& fileNamePrivate)
{
	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_BoardRAM = config::tBoardRAM(PTreeConfig);

	boost::property_tree::ptree PTreeDevice;
	boost::property_tree::json_parser::read_json(fileNameDevice, PTreeDevice);
	m_Device = share_config::tDevice(PTreeDevice);

	boost::property_tree::ptree PTreePrivate;
	boost::property_tree::json_parser::read_json(fileNamePrivate, PTreePrivate);
	m_Email = config::tEmail(PTreePrivate);
}

}
