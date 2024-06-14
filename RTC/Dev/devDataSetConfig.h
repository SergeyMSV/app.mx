#pragma once

#include <shareConfig.h>

#include <unordered_map>
#include <string>

namespace dev
{

class tDataSetConfig
{
public:
	using tThermoDallasMap = std::unordered_map<std::string, std::string>;

private:
	const std::string m_FileNameMXDev;

	std::string m_DallasPortID;
	share_config::tPipe m_Pipe;
	tThermoDallasMap m_ThermoDallasMap;

public:
	tDataSetConfig(const std::string& fileNameMX, const std::string& fileNameMXDev);

	void Load(const std::vector<std::string>& thermoIDs);

	std::string GetDallasPortID() const { return m_DallasPortID; }
	share_config::tPipe GetPipe() { return m_Pipe; }
	tThermoDallasMap GetThermoDallasMap() { return m_ThermoDallasMap; }
};

}
