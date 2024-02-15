#include "devDataSetConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tDataSetConfig g_Settings;

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNamePrivate)
	:m_ConfigFileName(fileNameConfig)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTree);

	boost::property_tree::ptree PTreePrivate;
	boost::property_tree::json_parser::read_json(fileNamePrivate, PTreePrivate);
	m_Platform = share_config::tPlatform(PTreePrivate);

	m_OutGNSS = config::tOutGNSS(PTree);
	m_SerialPort = config::tSerialPortGNSS(PTree, m_Platform.ID);
}

mod::tGnssTaskScript tDataSetConfig::GetTaskScript(const std::string& id) const
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(m_ConfigFileName, PTree);

	mod::tGnssTaskScript Script;

	for (auto& task : PTree.get_child("task"))
	{
		std::string TaskID = task.second.get<std::string>("id");
		if (TaskID != id)
			continue;

		for (auto& i : task.second.get_child("seq"))
		{
			std::string Type = i.second.get<std::string>("Type");
			if (Type == "REQ")
			{
				auto Value = std::make_unique<mod::tGnssTaskScriptCmdREQ>();
				Value->Msg = i.second.get<std::string>("Msg");
				Value->RspHead = i.second.get<std::string>("RspHead");
				Value->RspBody = i.second.get<std::string>("RspBody");
				Value->CaseRspWrong = i.second.get<std::string>("CaseRspWrong");
				Value->RspWait_us = i.second.get<decltype(Value->RspWait_us)>("RspWait_us");
				Value->Pause_us = i.second.get<decltype(Value->Pause_us)>("Pause_us");
				Script.push_back(std::move(Value));
			}
			else if (Type == "GPI")
			{
				auto Value = std::make_unique<mod::tGnssTaskScriptCmdGPI>();
				Value->ID = i.second.get<std::string>("ID");
				Value->State = i.second.get<decltype(Value->State)>("State");
				Value->Wait_us = i.second.get<decltype(Value->Wait_us)>("Wait_us");
				Script.push_back(std::move(Value));
			}
			else if (Type == "GPO")
			{
				auto Value = std::make_unique<mod::tGnssTaskScriptCmdGPO>();
				Value->ID = i.second.get<std::string>("ID");
				Value->State = i.second.get<decltype(Value->State)>("State");
				Value->Pause_us = i.second.get<decltype(Value->Pause_us)>("Pause_us");
				Script.push_back(std::move(Value));
			}
		}
	}

	return Script;
}

mod::tGnssSettingsNMEA tDataSetConfig::GetSettingsNMEA() const
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(m_ConfigFileName, PTree);

	auto ConfNMEA = PTree.get_child_optional("NMEA");
	if (!ConfNMEA.has_value())
		return {};

	auto& PTreeNMEA = ConfNMEA.get();

	mod::tGnssSettingsNMEA Settings;
	Settings.PeriodMax = PTreeNMEA.get<std::uint32_t>("PeriodMAX_us");
	Settings.LatLonFract = PTreeNMEA.get<std::uint8_t>("LatLonFract");

	for (auto& msg : PTreeNMEA.get_child("msgs"))
	{
		Settings.MsgLast = msg.second.get_value<std::string>();
	}

	return Settings;
}

}