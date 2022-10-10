#pragma once

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tEmail
{
	std::string To;
	uint32_t Period = 0;//in seconds, min limit set in c_tor

	tEmail() = default;
	explicit tEmail(boost::property_tree::ptree pTree);

	bool IsWrong() const;
};

struct tGNSS
{
	std::string Path;
	std::string Prefix;
	uint8_t QtyMax = 0;

	tGNSS() = default;
	explicit tGNSS(boost::property_tree::ptree pTree);

	bool IsWrong() const;
};

struct tPicture
{
	std::string Path;
	std::string Prefix;
	uint8_t QtyMax = 0;

	tPicture() = default;
	explicit tPicture(boost::property_tree::ptree pTree);

	bool IsWrong() const;
};

}

class tDataSetConfig
{
	config::tEmail m_Email;
	config::tGNSS m_GNSS;
	config::tPicture m_Picture;

public:
	explicit tDataSetConfig(const std::string& fileName);

	config::tEmail GetEmail() const { return m_Email; }
	config::tGNSS GetGNSS() const { return m_GNSS; }
	config::tPicture GetPicture() const { return m_Picture; }
};

}