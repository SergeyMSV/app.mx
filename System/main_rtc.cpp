#include <utilsLinux.h>
#include <utilsPath.h>
#include <utilsString.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

//#define LINUX_TEST

#ifdef LINUX_TEST
#include <iostream>
#endif // LINUX_TEST

bool CheckRTC(int id, int count, int pause)
{
	for (int i = 0; i < count; ++i)
	{
		std::filesystem::path Path = utils::path::GetPathNormal("/dev/rtc" + std::to_string(id));
#if defined(_WIN32)
		if (std::filesystem::exists(Path))
#else // _WIN32
		if (std::filesystem::exists(Path) && std::filesystem::is_character_file(Path))
#endif // _WIN32
			return true;
		std::this_thread::sleep_for(std::chrono::seconds(pause));
	}
	return false;
}

bool FindInLog(const std::string& cmd, const std::string& rsp)
{
	std::string Rsp = utils::linux::CmdLine(cmd);
	std::vector<std::string_view> Lines = utils::string::SplitTrimString(Rsp, '\n');
	for (const auto& i : Lines)
	{
		if (i.find(rsp) != std::string::npos)
			return true;
	}
	return false;
}

void AjustRTC(int id)
{
	utils::linux::CmdLine("hwclock --rtc /dev/rtc" + std::to_string(id) + " -w --noadjfile --utc");
	std::cerr << "rtc" << id << " has been adjusted\n";
}

bool AjustRTC(int id, int count, int pause)
{
	for (int i = 0; i < count; ++i)
	{
		if (FindInLog("journalctl -u systemd-timesyncd --grep \"Initial clock synchronization\" | grep \"UTC\"", "Initial clock synchronization"))
		{
			AjustRTC(id);
			return true;
		}
#ifdef LINUX_TEST
		if (FindInLog("journalctl --since today -u systemd-timesyncd --no-pager | grep \"synchronization\" | grep \"Initial\"", " Initial synchronization to time server"))
		{
			std::cout << "hwclock ajusted 2\n";
			return true;
		}
#endif // LINUX_TEST
		std::this_thread::sleep_for(std::chrono::seconds(pause));
	}
	return false;
}

void ThreadRTC()
{
	constexpr int RtcID = 1;

	if (!CheckRTC(RtcID, 10, 1))
	{
		std::cerr << "rtc" << RtcID << " is not available\n";
		std::this_thread::sleep_for(std::chrono::seconds(600));
		return;
	}

	if (!AjustRTC(RtcID, 6, 5)) // 6 * 5 = 30 seconds (every 5 seconds within first 30 seconds)
	{
		if (!AjustRTC(RtcID, 30, 10)) // 30 * 10 = 300 seconds (every 10 seconds within next 5 minutes)
		{
			while (!AjustRTC(RtcID, 1, 60)); // every minute until the initial synchronization
		}
	}

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(600)); // [#]

		std::chrono::seconds TimeSys = std::chrono::seconds(utils::linux::GetTimeSystem());
		if (!TimeSys.count())
			continue;

		std::chrono::seconds TimeRTC = std::chrono::seconds(utils::linux::GetTimeRTC(RtcID));
		if (!TimeRTC.count())
			continue;

		std::int64_t Diff = std::abs(TimeSys.count() - TimeRTC.count());
		if (Diff > 1) // [#] in seconds
			AjustRTC(RtcID);
	}
}

// # hwclock
// 2026-07-13 17:36:44.751250+00:00
// 
// # hwclock -f /dev/rtc1
// 2026-07-13 17:36:17.043334+00:00
// 
// # hwclock -f /dev/rtc1
// hwclock: ioctl(RTC_RD_TIME) to /dev/rtc1 to read the time failed: Invalid argument
// 
// # hwclock -f /dev/rtc3
// hwclock: Cannot access the Hardware Clock via any known method.
// hwclock: Use the --verbose option to see the details of our search for an access method.

// Set any time into RTC1
// # hwclock --set --date "2026-08-01 14:35:00" --rtc /dev/rtc1 --noadjfile --utc
// # hwclock --set --date "2024-09-05 14:35:00" --rtc /dev/rtc1 --noadjfile --utc

// # date -u +"%Y-%m-%d %H:%M:%S"
// 2026-09-05 00:12:06
// 
// # date -u +"%Y-%m-%d %H:%M:%S.%6N"
// 2026-09-05 00:26:49.261542

// # journalctl - u systemd - timesyncd --grep "Initial clock synchronization" | grep "UTC"
// Sep 06 21:40:58 opio systemd-timesyncd[167]: Initial clock synchronization to Sun 2026-09-06 21:40:58.513426 UTC.
