#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tSpyGNSS
{
	std::string Host;
	std::string TargetGLO;
	std::string TargetGPS;
	uint32_t Period = 0;//in seconds, min limit set in c_tor
	uint32_t RepPeriod = 0;//in seconds, min limit set in c_tor
	uint32_t RepQty = 0;

	tSpyGNSS() = default;
	explicit tSpyGNSS(boost::property_tree::ptree pTree);

	bool IsWrong();
};

struct tSpyOutGLO : public share_config::tOutFile
{
	tSpyOutGLO() = default;
	explicit tSpyOutGLO(boost::property_tree::ptree pTree)
		:share_config::tOutFile("out_glo", pTree)
	{}
};

struct tSpyOutGPS : public share_config::tOutFile
{
	tSpyOutGPS() = default;
	explicit tSpyOutGPS(boost::property_tree::ptree pTree)
		:share_config::tOutFile("out_gps", pTree)
	{}
};

}

class tDataSetConfig
{
	share_config::tDevice m_Device;
	config::tSpyGNSS m_SpyGNSS;
	config::tSpyOutGLO m_SpyOutGLO;
	config::tSpyOutGPS m_SpyOutGPS;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice);

	share_config::tDevice GetDevice() const { return m_Device; }
	config::tSpyGNSS GetSpyGNSS() const { return m_SpyGNSS; }
	config::tSpyOutGLO GetSpyOutGLO() const { return m_SpyOutGLO; }
	config::tSpyOutGPS GetSpyOutGPS() const { return m_SpyOutGPS; }
};

}