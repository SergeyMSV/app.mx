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

}