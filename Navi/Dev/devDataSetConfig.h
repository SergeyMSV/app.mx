#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

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
	explicit tEmail(const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tGNSS : share_config::tOutFile
{
	tGNSS() = default;
	explicit tGNSS(const boost::property_tree::ptree& pTree)
		:share_config::tOutFile("gnss", pTree)
	{}
};

struct tPicture : share_config::tOutFile
{
	tPicture() = default;
	explicit tPicture(const boost::property_tree::ptree& pTree)
		:share_config::tOutFile("picture", pTree)
	{}
};

}

class tDataSetConfig
{
	share_config::tDevice m_Device;
	config::tEmail m_Email;
	config::tGNSS m_GNSS;
	config::tPicture m_Picture;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice, const std::string& fileNamePrivate);

	share_config::tDevice GetDevice() const { return m_Device; }
	config::tEmail GetEmail() const { return m_Email; }
	config::tGNSS GetGNSS() const { return m_GNSS; }
	config::tPicture GetPicture() const { return m_Picture; }
};

}