#include <utilsLinux.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

constexpr char g_PathMXSetupSchedule[] = "/etc/cron.d/mxsetupschedule";

void MakeMXSetupSchedule(const std::string& appPath, const std::string& appArg)
{
	std::string PathFileMXScheduleSetup = utils::linux::GetPath(g_PathMXSetupSchedule);

	std::vector<std::string> Lines;

	Lines.push_back("#!/bin/bash");
	Lines.push_back("@reboot root " + appPath + " " + appArg);

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
	std::string Path = utils::linux::GetPath(g_PathMXSetupSchedule);
	std::filesystem::remove(Path);
}
