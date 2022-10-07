#include "devDataSetConfig.h"

#include <utilsLinux.h>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

namespace config
{

tDevice::tDevice(boost::property_tree::ptree pTree)
{
	Type = pTree.get<std::string>("device.type");
	Version = utils::tVersion(pTree.get<std::string>("firmware.version"));
}

tEmail::tEmail(boost::property_tree::ptree pTree)
{
	To = pTree.get<std::string>("mxnavi.email.to");
	Period = pTree.get<uint32_t>("mxnavi.email.period");
}

bool tEmail::IsWrong() const
{
	return To.empty() || Period == 0; //[TBD] check if email is correct
}

tGNSS::tGNSS(boost::property_tree::ptree pTree)
{
	std::string PathRaw = pTree.get<std::string>("gnss.path");
	Path = utils::linux::GetPath(PathRaw);
	Prefix = pTree.get<std::string>("gnss.prefix");
	QtyMax = pTree.get<uint8_t>("gnss.qtyMax");
}

bool tGNSS::IsWrong() const
{
	return Path.empty() || Prefix.empty() || QtyMax == 0;;
}

tPicture::tPicture(boost::property_tree::ptree pTree)
{
	std::string PathRaw = pTree.get<std::string>("picture.path");
	Path = utils::linux::GetPath(PathRaw);
	Prefix = pTree.get<std::string>("picture.prefix");
	QtyMax = pTree.get<uint8_t>("picture.qtyMax");
}

bool tPicture::IsWrong() const
{
	return Path.empty() || Prefix.empty() || QtyMax == 0;
}

}

tDataSetConfig::tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice, const std::string& fileNamePrivate)
{
	boost::property_tree::ptree PTreeConfig;
	boost::property_tree::json_parser::read_json(fileNameConfig, PTreeConfig);
	m_GNSS = config::tGNSS(PTreeConfig);
	m_Picture = config::tPicture(PTreeConfig);

	boost::property_tree::ptree PTreeDevice;
	boost::property_tree::json_parser::read_json(fileNameDevice, PTreeDevice);
	m_Device = config::tDevice(PTreeDevice);

	boost::property_tree::ptree PTreePrivate;
	boost::property_tree::json_parser::read_json(fileNamePrivate, PTreePrivate);
	m_Email = config::tEmail(PTreePrivate);
}

}
