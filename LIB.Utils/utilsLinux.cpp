#include "utilsBase.h"
#include "utilsLinux.h"
#include "utilsPath.h"

#include <cerrno>
#include <cstdio>

#include <deque>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

//#define LIB_UTILS_LINUX_LOG

#if defined(LIB_UTILS_LINUX_LOG)
#include <iostream>
#endif

namespace utils
{

static std::string GetStringEnding(const std::string& pattern, const std::string& str)//utilsString
{
	std::size_t Pos = str.find(pattern);
	if (Pos == std::string::npos)
		return {};
	std::string Str = str.substr(Pos + pattern.size());
	std::string_view StrView = Str;
	StrView.remove_prefix(std::min(StrView.find_first_not_of(" "), StrView.size()));
	return StrView.data();
}

namespace linux
{

#if defined(_WIN32)
static std::string CmdLineWinTest(const std::string& cmd);
#endif

std::string CmdLine(const std::string& cmd)
{
#if defined(_WIN32)
	return CmdLineWinTest(cmd);
#else
	FILE* File = popen(cmd.c_str(), "r");//File = popen("/bin/ls /etc/", "r");
	if (File == NULL)
		return {};

	std::string CmdRsp;

	char DataChunk[1035];
	while (fgets(DataChunk, sizeof(DataChunk), File) != NULL)
	{
		CmdRsp += DataChunk;
	}

	pclose(File);

	if (!CmdRsp.empty() && CmdRsp.back() == '\n')//Removes last '\n'
		CmdRsp.pop_back();

	return CmdRsp;
#endif
}

double GetUptime()
{
	std::filesystem::path Path = path::GetPathNormal("/proc/uptime");
	std::fstream File(Path, std::ios::in);
	if (!File.good())
		return {};

	double UptimeSeconds;
	File >> UptimeSeconds;

	File.close();

	return UptimeSeconds;
}

std::string UptimeToString(double uptime)
{
	int Utime_Day = static_cast<int>(uptime / 86400);
	int UptimeRemove = Utime_Day * 86400;
	int Utime_Hour = static_cast<int>((uptime - UptimeRemove) / 3600);
	UptimeRemove += Utime_Hour * 3600;
	int Utime_Min = static_cast<int>((uptime - UptimeRemove) / 60);
	UptimeRemove += Utime_Min * 60;
	int Utime_Sec = static_cast<int>(uptime - UptimeRemove);

	std::stringstream SStr;
	SStr << Utime_Day << " days ";
	SStr << std::setfill('0') << std::setw(2) << Utime_Hour << ":";
	SStr << std::setfill('0') << std::setw(2) << Utime_Min << ":";
	SStr << std::setfill('0') << std::setw(2) << Utime_Sec;

	return SStr.str();
}

std::string GetUptimeString()
{
	return UptimeToString(GetUptime());
}

tCpuInfo GetCpuInfo()
{
	std::filesystem::path Path = path::GetPathNormal("/proc/cpuinfo");
	std::fstream File(Path, std::ios::in);
	if (!File.good())
		return {};

	tCpuInfo CpuInfo{};

	std::deque<std::string> Strings;

	while (!File.eof())
	{
		std::string Line;
		std::getline(File, Line);
		Line.erase(std::remove_if(Line.begin(), Line.end(), [](char ch) { return ch == '\t'; }), Line.end());

		std::size_t Pos = Line.find(":", 0);
		std::string PrmName = Line.substr(0, Pos);

		auto GetValueString = [&Pos](const std::string& a_line)
		{
			std::string Value = a_line.substr(Pos, a_line.size());
			Value.erase(Value.begin(), std::find_if(Value.begin(), Value.end(), [](char ch) { return ch != ' ' && ch != ':'; }));
			return Value;
		};

		if (PrmName == "model name")
		{
			CpuInfo.ModelName = GetValueString(Line);
		}
		else if (PrmName == "BogoMIPS")
		{
			errno = 0;
			std::string Value = GetValueString(Line);
			double Num = strtod(Value.c_str(), nullptr);
			if (Num > 0 && errno != ERANGE)
				CpuInfo.BogoMIPS = Num;
			errno = 0;
		}
		else if (PrmName == "Hardware")
		{
			CpuInfo.Hardware = GetValueString(Line);
		}
	}

	File.close();

	return CpuInfo;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions for tests
#if defined(_WIN32)
static std::string CmdLineWinTest(const std::string& cmd)
{
	if (cmd == "free")
	{
		return
			"               total        used        free      shared  buff / cache   available\n\
Mem:          492116       33240      129108        1184      329768      445984\n\
Swap:              0           0           0";
	}

	std::string CmdValue = GetStringEnding("cat", cmd);
	if (!CmdValue.empty())
	{
		std::filesystem::path Path = path::GetPathNormal(CmdValue);
		if (Path.empty())
			return {};

		std::fstream File(Path, std::ios::in);
		if (!File.good())
			return {};

		std::string Data;

		while (!File.eof())
		{
			std::string Line;
			std::getline(File, Line);
			Data += Line + "\n";
		}

		File.close();

		return Data;
	}

	return {};
}
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
}

}
