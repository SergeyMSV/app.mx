#include "shareLANTag.h"

namespace share
{

tLANTag::tLANTag(const std::string& platformID, const std::string& hostName)
	:PlatformID(platformID), HostName(hostName)
{
}

tLANTag::tLANTag(const boost::property_tree::ptree & pTree)
{
	PlatformID = pTree.get<std::string>("platform_id");
}

std::string tLANTag::ToJSON() const
{
	std::string Str = "{";
	Str += "\"platform_id\":\"" + PlatformID + "\",";
	Str += "\"hostname\":\"" + HostName + "\"";
	Str += "}";
	return Str;
}

}
