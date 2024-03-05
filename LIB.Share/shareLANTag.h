#pragma once
#include <string>
#include <boost/property_tree/ptree.hpp>

namespace share
{

struct tLANTag
{
	std::string PlatformID;
	std::string HostName;

	tLANTag() = default;
	tLANTag(const std::string& platformID, const std::string& hostName);
	explicit tLANTag(const boost::property_tree::ptree & pTree);

	std::string ToJSON() const;
};

}
