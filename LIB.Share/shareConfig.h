#pragma once

#include <utilsBase.h>

#include <boost/property_tree/ptree.hpp>

namespace share_config
{

struct tDevice
{
	std::string Type;
	utils::tVersion Version;

	tDevice() = default;
	explicit tDevice(const boost::property_tree::ptree& pTree);
};

struct tOutFile
{
	std::string Path;
	std::string Prefix;
	uint8_t QtyMax = 0;

	tOutFile() = default;
	tOutFile(const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tOutFileCap : public tOutFile
{
	uint32_t Capacity = 0;

	tOutFileCap() = default;
	tOutFileCap(const std::string& baseName, const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tSerialPort
{
	std::string ID;
	std::uint32_t BR = 0;

	tSerialPort() = default;
	tSerialPort(const std::string & baseName, const boost::property_tree::ptree & pTree);

	bool IsWrong() const;
};

}