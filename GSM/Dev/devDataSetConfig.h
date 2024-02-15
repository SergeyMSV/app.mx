#pragma once

#include <devConfig.h>

#include <modGnss.h>

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tOutGNSS : public share_config::tOutFile
{
	tOutGNSS() = default;
	explicit tOutGNSS(boost::property_tree::ptree pTree)
		:share_config::tOutFile("out", pTree)
	{}
};

struct tSerialPortGNSS : public share_config::tSerialPort
{
	tSerialPortGNSS() = default;
	tSerialPortGNSS(boost::property_tree::ptree pTree, const std::string& platformID)
		:share_config::tSerialPort("serial_port", platformID, pTree)
	{}
};

}

class tDataSetConfig
{
	share_config::tPlatform m_Platform;
	config::tOutGNSS m_OutGNSS;
	config::tSerialPortGNSS m_SerialPort;

	std::string m_ConfigFileName;

public:
	tDataSetConfig() = default;
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNamePrivate);

	config::tOutGNSS GetOutGNSS() const { return m_OutGNSS; }
	config::tSerialPortGNSS GetSerialPortGNSS() const { return  m_SerialPort; }
	mod::tGnssTaskScript GetTaskScript(const std::string& id) const;
	mod::tGnssSettingsNMEA GetSettingsNMEA() const;
};

extern tDataSetConfig g_Settings;

}