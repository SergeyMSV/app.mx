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

namespace linux
{

#if defined(_WIN32) && defined(LIB_UTILS_LINUX_CMDLINEWINTEST)
std::string CmdLineWinTest(const std::string& cmd);
#endif

std::string CmdLine(const std::string& cmd)
{
#if defined(_WIN32)
#if defined(LIB_UTILS_LINUX_CMDLINEWINTEST)
	return CmdLineWinTest(cmd);
#else // LIB_UTILS_LINUX_CMDLINEWINTEST
	return "";
#endif // LIB_UTILS_LINUX_CMDLINEWINTEST
#else // _WIN32
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
#endif // _WIN32
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

static std::time_t GetTime(const std::string& cmd)
{
	std::istringstream SStr(utils::linux::CmdLine(cmd));
	std::tm DTime{};
	SStr >> std::get_time(&DTime, "%Y-%m-%d %H:%M:%S");
	if (SStr.fail())
		return {};
	std::time_t Time = 0;
#if defined(_WIN32) || defined(_MSC_VER)
	Time = _mkgmtime(&DTime);
#elif defined(__unix__) || defined(__APPLE__)
	Time = timegm(&DTime);
#else
#error "Unsupported platform: no UTC mktime equivalent"
#endif
	if (Time == static_cast<std::time_t>(-1))
		return {};
	return Time;
}

std::time_t GetTimeSystem()
{
	return GetTime("date -u +\"%Y-%m-%d %H:%M:%S\""); // UTC
}

std::time_t GetTimeRTC(std::uint8_t rtcID)
{
	return GetTime("hwclock -f /dev/rtc" + std::to_string(rtcID));
}

static std::string ReadFile(const std::filesystem::path& a_path)
{
	std::ifstream file(a_path);
	if (!file.good())
		return {};
	std::string value;
	file >> value;
	file.close();
	return value;
}

static std::uint8_t ReadFileReg(const std::filesystem::path& a_path)
{
	std::ifstream file(a_path, std::ios_base::binary);
	if (!file.good())
		return {};
	char buf[10]{};
	file.read(buf, sizeof(buf));
	std::streamsize bytesRead = file.gcount();
	file.close();
	return bytesRead == 1 ? buf[0] : 0;
}

std::vector<tHwmon> GetHwmon()
{
	std::vector<tHwmon> res;
	const std::filesystem::path path = utils::path::GetPathNormal("/sys/class/hwmon");
	std::error_code ec;
	for (const auto& entry : std::filesystem::directory_iterator(path, ec))
	{
		if (!entry.is_directory())
			continue;
		std::string dh = entry.path().filename().string();
		if (!dh.starts_with("hwmon"))
			continue;
		tHwmon hwmon{ .ID = dh };
		for (const auto& item : std::filesystem::directory_iterator(entry.path(), ec))
		{
			if (item.is_directory())
			{
				std::string dir = item.path().filename().string();
				if (dir == "of_node")
					hwmon.Reg = ReadFileReg(item.path() / "reg");
				continue;
			}

			std::string fh = item.path().filename().string();
			if (fh == "name")
			{
				hwmon.Name = ReadFile(item.path());
			}
			else if (fh == "label")
			{
				hwmon.Label = ReadFile(item.path());
			}
			else if (fh == "temp1_input")
			{
				hwmon.Temperature = ReadFile(item.path());
			}
			else if (fh == "humidity1_input")
			{
				hwmon.Humidity = ReadFile(item.path());
			}
		}
		res.push_back(std::move(hwmon));
	}

	return res;
}

}

}
