#pragma once

#include <string>
#include <vector>

#include <utilsPath.h>

#include <shareConfig.h>

namespace dev
{

namespace config
{
	struct tSensorProp
	{
		std::string ID;
		std::string Label;
	};
}

class tConfig
{
	share::config::tPlatform m_Platform;
	share::config::port::tUART_Config m_Dallas;
	std::vector<config::tSensorProp> m_SensorsProp;
	share::config::tOutFile m_OutFile;

public:
	explicit tConfig(const std::filesystem::path& a_path);

	share::config::port::tUART_Config GetDallas() const { return m_Dallas; }
	config::tSensorProp GetSensorProp(std::string a_romID) const;
	share::config::tOutFile GetOutFile() const { return m_OutFile; }
};

}
