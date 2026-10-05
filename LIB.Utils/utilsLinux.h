///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsLinux
// 2022-09-02 - 2026-10-05
// Standard ISO/IEC 114882, C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <vector>

#include <ctime>

namespace utils
{

namespace linux
{

std::string CmdLine(const std::string& cmd);

double GetUptime();
std::string GetUptimeString();

struct tCpuInfo
{
	std::string ModelName;
	double BogoMIPS = 0.0;
	std::string Hardware;

	tCpuInfo() = default;
	tCpuInfo(const std::string& modelName, double bogoMIPS, const std::string& hardware)
		: ModelName(modelName), BogoMIPS(bogoMIPS), Hardware(hardware)
	{}

	bool operator == (const tCpuInfo&) const = default;
	bool operator != (const tCpuInfo&) const = default;
};

tCpuInfo GetCpuInfo();

std::time_t GetTimeSystem();
std::time_t GetTimeRTC(std::uint8_t rtcID);

struct tHwmon
{
	std::string ID;
	std::string Name;
	std::string Label;
	std::string Temperature;
	std::string Humidity;
	std::uint8_t Reg;
};

std::vector<tHwmon> GetHwmon();

}

}
