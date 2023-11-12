#include "shareConfig.h"

#include <utilsPath.h>

#include <filesystem>

#include <boost/property_tree/json_parser.hpp>

namespace share_config
{

static std::string GetPostfix(const std::string& value)
{
	return  value.empty() ? "" : "_" + value;
}

tDevice::tDevice(const boost::property_tree::ptree& pTree)
{
	Type = pTree.get<std::string>("device.type");
	Version = utils::tVersion(pTree.get<std::string>("firmware.version"));
}

tDevice::tDevice(const std::string& type, const utils::tVersion& version)
	:Type(type), Version(version)
{
}

tPlatform::tPlatform(const boost::property_tree::ptree& pTree)
{
	ID = pTree.get<std::string>("platform.id");
}

tUpdateServer::tUpdateServer(const boost::property_tree::ptree& pTree)
{
	Host = pTree.get<std::string>("server.host");
	Target = pTree.get<std::string>("server.target");
	TargetList = pTree.get<std::string>("server.target_list");
}

tOutFile::tOutFile(const std::string& baseName, const boost::property_tree::ptree& pTree)
{
	std::filesystem::path PathRaw = pTree.get<std::string>(baseName + ".path");
	Path = utils::path::GetPathNormal(PathRaw).string();
	Prefix = pTree.get<std::string>(baseName + ".prefix");
	QtyMax = pTree.get<uint8_t>(baseName + ".qtyMax", 0);
}

bool tOutFile::IsWrong() const
{
	return Path.empty() || Prefix.empty();
}

tOutFileCap::tOutFileCap(const std::string& baseName, const boost::property_tree::ptree& pTree)
	:tOutFile(baseName, pTree)
{
	Capacity = pTree.get<uint32_t>(baseName + ".capacity");
}

bool tOutFileCap::IsWrong() const
{
	return tOutFile::IsWrong() || Capacity == 0;
}

tPipe::tPipe(const std::string& pipeName, const std::string& platformID, const boost::property_tree::ptree& pTree)
{
	Path = pTree.get<std::string>("pipe." + pipeName + GetPostfix(platformID), "");
}

bool tPipe::IsWrong() const
{
	return Path.empty();
}

tSerialPort::tSerialPort(const std::string& baseName, const std::string& platformID, const boost::property_tree::ptree& pTree)
{
	ID = pTree.get<std::string>(baseName + ".id" + GetPostfix(platformID));
	BR = pTree.get<uint32_t>(baseName + ".br");
}

bool tSerialPort::IsWrong() const
{
	return ID.empty() || BR == 0;
}

tSPIPort::tSPIPort(const std::string& portID, const std::string& baseName, const boost::property_tree::ptree& pTree)
	:ID(portID)
{
	Mode = pTree.get<std::uint8_t>(baseName + ".mode", 0);
	Bits = pTree.get<std::uint8_t>(baseName + ".bits", 0);
	Frequency_hz = pTree.get<std::uint32_t>(baseName + ".frequency_hz", 0);
	Delay_us = pTree.get<std::uint16_t>(baseName + ".delay_us", 0);
}

bool tSPIPort::IsWrong() const
{
	return ID.empty() || !Bits || !Frequency_hz;
}

tGPIOPort::tGPIOPort(const std::string& baseName, const boost::property_tree::ptree& pTree)
{
	ID = pTree.get<std::string>(baseName, "");
}

bool tGPIOPort::IsWrong() const
{
	return ID.empty();
}

}
