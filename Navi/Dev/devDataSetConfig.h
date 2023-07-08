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

struct tGNSS : public share_config::tOutFile
{
	tGNSS() = default;
	explicit tGNSS(const boost::property_tree::ptree& pTree)
		:share_config::tOutFile("gnss", pTree)
	{}
};

struct tPicture : public share_config::tOutFile
{
	tPicture() = default;
	explicit tPicture(const boost::property_tree::ptree& pTree)
		:share_config::tOutFile("picture", pTree)
	{}
};

struct tSpyOutGLO : public share_config::tOutFile
{
	tSpyOutGLO() = default;
	explicit tSpyOutGLO(const boost::property_tree::ptree& pTree)
		:share_config::tOutFile("mxspy_out_glo", pTree)
	{}
};

struct tSpyOutGPS : public share_config::tOutFile
{
	tSpyOutGPS() = default;
	explicit tSpyOutGPS(const boost::property_tree::ptree& pTree)
		:share_config::tOutFile("mxspy_out_gps", pTree)
	{}
};

struct tOutPicture : public share_config::tOutFile
{
	tOutPicture() = default;
	explicit tOutPicture(boost::property_tree::ptree pTree)
		:share_config::tOutFile("out_pic", pTree)
	{}
};

struct tLog : public share_config::tOutFileCap
{
	tLog() = default;
	explicit tLog(const boost::property_tree::ptree& pTree)
		:share_config::tOutFileCap("log", pTree)
	{}
};

}

class tDataSetConfig
{
	share_config::tDevice m_Device;
	config::tEmail m_Email;
	config::tGNSS m_GNSS;
	config::tPicture m_Picture;
	config::tSpyOutGLO m_SpyOutGLO;
	config::tSpyOutGPS m_SpyOutGPS;
	config::tOutPicture m_OutPicture;
	config::tLog m_Log;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice, const std::string& fileNamePrivate);

	share_config::tDevice GetDevice() const { return m_Device; }
	config::tEmail GetEmail() const { return m_Email; }
	config::tGNSS GetGNSS() const { return m_GNSS; }
	config::tPicture GetPicture() const { return m_Picture; }
	config::tSpyOutGLO GetSpyOutGLO() const { return m_SpyOutGLO; }
	config::tSpyOutGPS GetSpyOutGPS() const { return m_SpyOutGPS; }
	config::tOutPicture GetOutPicture() const { return m_OutPicture; }
	config::tLog GetLog() const { return m_Log; }
};

}