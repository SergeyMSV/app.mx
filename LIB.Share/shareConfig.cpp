#include "shareConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace share_config
{

tDevice::tDevice(const boost::property_tree::ptree& pTree)
{
	Type = pTree.get<std::string>("device.type");
	Version = utils::tVersion(pTree.get<std::string>("firmware.version"));
}

tOutFile::tOutFile(const std::string& baseName, const boost::property_tree::ptree& pTree)
{
	std::string PathRaw = pTree.get<std::string>(baseName + ".path");
	Path = utils::linux::GetPath(PathRaw);
	Prefix = pTree.get<std::string>(baseName + ".prefix");
	QtyMax = pTree.get<uint8_t>(baseName + ".qtyMax");
}

bool tOutFile::IsWrong() const
{
	return Path.empty() || Prefix.empty() || QtyMax == 0;
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

tSerialPort::tSerialPort(const std::string& baseName, const boost::property_tree::ptree& pTree)
{
	ID = pTree.get<std::string>(baseName + ".id");
	BR = pTree.get<uint32_t>(baseName + ".br");

#if defined(_WIN32)
	ID = pTree.get<std::string>(baseName + ".id_win");
#endif
}

bool tSerialPort::IsWrong() const
{
	return ID.empty() || BR == 0;
}

}
