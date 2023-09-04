#include "devDataSetConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tDevice::tDevice(boost::property_tree::ptree a_PTree)
{
	Model = a_PTree.get<std::string>("device.model");
}

}

tDataSetConfig g_Settings;

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNamePrivate)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTree);

	boost::property_tree::ptree PTreePrivate;
	boost::property_tree::json_parser::read_json(fileNamePrivate, PTreePrivate);
	m_Platform = share_config::tPlatform(PTreePrivate);

	m_SerialPortCtrl = config::tSerialPortCtrl(PTree, m_Platform.ID);
	m_SerialPortData = config::tSerialPortData(PTree, m_Platform.ID);
	m_OutPicture = config::tOutPicture(PTree);

	auto Str = PTree.get<std::string>("camera.resolution");
	m_Camera.Resolution = utils::packet_CameraVC0706::ToResolution(Str);
	m_Camera.CheckPresencePeriod_ms = PTree.get<std::uint32_t>("camera.check_presence_period_ms");
	m_Camera.ImagePeriod_ms = PTree.get<std::uint32_t>("camera.image_period_ms");
	m_Camera.ImageChunkSize = PTree.get<std::uint32_t>("camera.image_chunk_size");
	m_Camera.ImageChunkDelayFromReq_us = PTree.get<std::uint32_t>("camera.image_chunk_delay_from_req_us");

	m_Camera.SetPortCtrlBR(m_SerialPortCtrl.BR);
	m_Camera.SetPortDataBR(m_SerialPortData.BR);
}

}