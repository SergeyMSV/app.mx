#include "shareConfig.h"

#include <utilsPath.h>

#include <filesystem>

#include <boost/property_tree/json_parser.hpp>

namespace share
{
namespace config
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

tID::tID(const boost::property_tree::ptree& pTree, const std::string& name)
{
	ID = pTree.get<std::string>(name);
}

tUpdateServer::tUpdateServer(const boost::property_tree::ptree& pTree)
{
	Host = pTree.get<std::string>("server.host");
	Target = pTree.get<std::string>("server.target");
	TargetList = pTree.get<std::string>("server.target_list");
}

tOutFile::tOutFile(const std::string& baseName, const boost::property_tree::ptree& pTree, const std::string& prefix)
{
	std::filesystem::path PathRaw = pTree.get<std::string>(baseName + ".path");
	Path = utils::path::GetPathNormal(PathRaw).string();
	Prefix = prefix;
	QtyMax = pTree.get<uint8_t>(baseName + ".qtyMax", 0);
}

bool tOutFile::IsWrong() const
{
	return Path.empty();
}

tOutFileCap::tOutFileCap(const std::string& baseName, const boost::property_tree::ptree& pTree, const std::string& prefix)
	:tOutFile(baseName, pTree, prefix)
{
	Capacity = pTree.get<uint32_t>(baseName + ".capacity");
}

bool tOutFileCap::IsWrong() const
{
	return tOutFile::IsWrong() || Capacity == 0;
}

tPipe::tPipe(const std::string& pipeName, const boost::property_tree::ptree& pTree)
{
	Path = pTree.get<std::string>("pipe." + pipeName, "");
}

bool tPipe::IsWrong() const
{
	return Path.empty();
}

namespace port
{

tUART_Config::tUART_Config(const std::string& baseName, const std::string& platformID, const boost::property_tree::ptree& pTree)
{
	ID = pTree.get<std::string>(baseName + ".id" + GetPostfix(platformID), "");
	if (ID.empty())
		ID = pTree.get<std::string>(baseName + ".id", "");
	BR = pTree.get<uint32_t>(baseName + ".br", 0);
}

bool tUART_Config::IsWrong() const
{
	return ID.empty() || BR == 0;
}

tSPI_Config::tSPI_Config(const std::string& portID, const std::string& baseName, const boost::property_tree::ptree& pTree)
	:ID(portID)
{
	Mode = pTree.get<std::uint8_t>(baseName + ".mode", 0);
	Bits = pTree.get<std::uint8_t>(baseName + ".bits", 0);
	Frequency_hz = pTree.get<std::uint32_t>(baseName + ".frequency_hz", 0);
	Delay_us = pTree.get<std::uint16_t>(baseName + ".delay_us", 0);
}

bool tSPI_Config::IsWrong() const
{
	return ID.empty() || !Bits || !Frequency_hz;
}

tGPIO_Config::tGPIO_Config(const std::string& baseName, const boost::property_tree::ptree& pTree)
{
	ID = pTree.get<std::string>(baseName, "");
}

bool tGPIO_Config::IsWrong() const
{
	return ID.empty();
}

tIP_Config::tIP_Config(const std::string& baseName, const boost::property_tree::ptree& pTree)
{
	Value = pTree.get<std::uint16_t>(baseName, 0);
}

bool tIP_Config::IsWrong() const
{
	return Value == 0;
}

}
}
}
