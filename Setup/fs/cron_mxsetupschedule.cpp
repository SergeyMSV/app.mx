#include <utilsLinux.h>
#include <utilsPath.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

constexpr char g_PathMXSetupSchedule[] = "/etc/cron.d/mxsetupschedule";

void MakeMXSetupSchedule(const std::string& appPath, const std::string& appArg, const std::string& appLogID)
{
	std::string PathFileMXScheduleSetup = utils::path::GetPathNormal(g_PathMXSetupSchedule).string();

	std::vector<std::string> Lines;

	Lines.push_back("#!/bin/bash");
	// Pipe the output of the cron command through logger so they end up in the syslog.
	// stderr is to be forwarded to stdout(2>&1)
	Lines.push_back("@reboot root " + appPath + " " + appArg + " 2>&1 | logger -t " + appLogID);

	std::fstream File(PathFileMXScheduleSetup, std::ios::out | std::ios::binary);
	if (!File.good())
		return;

	for (auto& i : Lines)
	{
		File.write(i.c_str(), i.size());
		File.put(0x0A);
	}
	File.close();

	utils::linux::CmdLine("chmod 544 " + PathFileMXScheduleSetup);
}

void RemoveMXSetupSchedule()
{
	std::filesystem::path Path = utils::path::GetPathNormal(g_PathMXSetupSchedule);
	std::filesystem::remove(Path);
}
