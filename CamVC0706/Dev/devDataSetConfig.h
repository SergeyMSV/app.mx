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
	explicit tSerialPortCtrl(boost::property_tree::ptree pTree)
		:share_config::tSerialPort("serial_port_ctrl", pTree)
	{}
};

struct tSerialPortData : public share_config::tSerialPort
{
	tSerialPortData() = default;
	explicit tSerialPortData(boost::property_tree::ptree pTree)
		:share_config::tSerialPort("serial_port_data", pTree)
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
	config::tDevice m_Device;
	config::tSerialPortCtrl m_SerialPortCtrl;
	config::tSerialPortData m_SerialPortData;
	config::tOutPicture m_OutPicture;

	mod::tCameraVC0706Settings m_Camera;

	std::string m_ConfigFileName;

public:
	tDataSetConfig() = default;
	explicit tDataSetConfig(const std::string& fileName);

	config::tDevice GetDevice() { return m_Device; }
	config::tOutPicture GetOutPicture() const { return m_OutPicture; }
	config::tSerialPortCtrl GetSerialPortCtrl() const { return  m_SerialPortCtrl; }
	config::tSerialPortData GetSerialPortData() const { return  m_SerialPortData; }
	mod::tCameraVC0706Settings GetCamera() { return m_Camera; }

	//std::string GetConfigFileName() const { return m_ConfigFileName; }
};

extern tDataSetConfig g_Settings;

}