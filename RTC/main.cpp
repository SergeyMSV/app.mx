#include <utilsExits.h>
#include <utilsLinux.h>

#include <chrono>
#include <sstream>
#include <string>
#include <thread>

static std::chrono::seconds GetDateTime(const std::string& cmd)
{
	std::istringstream SStr(utils::linux::CmdLine(cmd));
	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> TimePoint;
	std::chrono::from_stream(SStr, "%Y-%m-%d %H:%M:%S", TimePoint);
	return TimePoint.time_since_epoch();
}

int main()
{
	while (true)
	{
		std::chrono::seconds TimeSys = GetDateTime("date -u +\"%Y-%m-%d %H:%M:%S\"");
		if (TimeSys.count())
		{
			std::chrono::seconds TimeRTC1 = GetDateTime("hwclock -f /dev/rtc1");
			if (TimeRTC1.count())
			{
				int64_t Diff = std::abs(TimeSys.count() - TimeRTC1.count());
				if (Diff > 1) // 1 s
					utils::linux::CmdLine("hwclock --rtc /dev/rtc1 -w --noadjfile --utc");
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	return utils::exit_code::EX_OK;
}

// root@host:~# hwclock
// 2026-07-13 17:36:44.751250+00:00
// root@host:~# hwclock -f /dev/rtc1
// 2026-07-13 17:36:17.043334+00:00
// root@host:~# hwclock -f /dev/rtc3
// hwclock: Cannot access the Hardware Clock via any known method.
// hwclock: Use the --verbose option to see the details of our search for an access method.

// Set any time into RTC1
// root@host:~# hwclock --set --date "2024-09-05 14:35:00" --rtc /dev/rtc1 --noadjfile --utc

//root@host:~# date -u +"%Y-%m-%d %H:%M:%S"
//2026-09-05 00:12:06
//root@host:~# date -u +"%Y-%m-%d %H:%M:%S.%6N"
//2026-09-05 00:26:49.261542
