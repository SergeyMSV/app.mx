#pragma once

#include <devConfig.h>

#include <modCameraVC0706.h>

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tDevice
{
	std::string Model;

	tDevice() = default;
	explicit tDevice(boost::property_tree::ptree a_PTree);
};

struct tSerialPortCtrl : public share_config::tSerialPort
{
	tSerialPortCtrl() = default;
	tSerialPortCtrl(boost::property_tree::ptree pTree, const std::string& platformID)
		:share_config::tSerialPort("serial_port_ctrl", platformID, pTree)
	{}
};

struct tSerialPortData : public share_config::tSerialPort
{
	tSerialPortData() = default;
	tSerialPortData(boost::property_tree::ptree pTree, const std::string& platformID)
		:share_config::tSerialPort("serial_port_data", platformID, pTree)
	{}
};

struct tOutPicture : public share_config::tOutFile
{
	tOutPicture() = default;
	explicit tOutPicture(boost::property_tree::ptree pTree)
		:share_config::tOutFile("out", pTree)
	{}
};

}

class tDataSetConfig
{
	share_config::tPlatform m_Platform;
	config::tSerialPortCtrl m_SerialPortCtrl;
	config::tSerialPortData m_SerialPortData;
	config::tOutPicture m_OutPicture;
	mod::tCameraVC0706Settings m_Camera;

public:
	tDataSetConfig() = default;
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNamePrivate);

	config::tOutPicture GetOutPicture() const { return m_OutPicture; }
	config::tSerialPortCtrl GetSerialPortCtrl() const { return  m_SerialPortCtrl; }
	config::tSerialPortData GetSerialPortData() const { return  m_SerialPortData; }
	mod::tCameraVC0706Settings GetCamera() { return m_Camera; }
};

extern tDataSetConfig g_Settings;

}