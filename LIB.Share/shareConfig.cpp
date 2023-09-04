#include "shareConfig.h"

#include <utilsPath.h>

#include <filesystem>

#include <boost/property_tree/json_parser.hpp>

namespace share_config
{

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

tSerialPort::tSerialPort(const std::string& baseName, const std::string& platformID, const boost::property_tree::ptree& pTree)
{
	ID = pTree.get<std::string>(baseName + ".id" + (platformID.empty() ?  "" : "_" + platformID));
	BR = pTree.get<uint32_t>(baseName + ".br");
}

bool tSerialPort::IsWrong() const
{
	return ID.empty() || BR == 0;
}

}
