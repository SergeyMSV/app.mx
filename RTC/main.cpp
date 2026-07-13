#include <utilsExits.h>
#include <utilsLinux.h>

#include <chrono>
#include <format>
#include <sstream>
#include <string>
#include <thread>

#include <iostream> // [TEST]

int main()
{
	const int DateTimeNeededSize = 19; // 2026-07-13 17:36:17
	while (true)
	{
		// root@host:~# hwclock
		// 2026-07-13 17:36:44.751250+00:00
		// root@host:~# hwclock -f /dev/rtc1
		// 2026-07-13 17:36:17.043334+00:00
		// root@host:~# hwclock -f /dev/rtc3
		// hwclock: Cannot access the Hardware Clock via any known method.
		// hwclock: Use the --verbose option to see the details of our search for an access method.
		//std::string DataRaw = utils::linux::CmdLine("hwclock");
		std::string DataRaw = "2026-07-13 17:36:44.751250+00:00";
		if (DataRaw.size() != 32 && DataRaw[20] != '.') // 2026-07-13 17:36:17.043334+00:00
			return utils::exit_code::EX_IOERR;
		std::istringstream SStr(DataRaw);
		std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> TimePoint;

		// Format: %Y-%m-%d %H:%M:%S.%f%Ez
		if (std::chrono::from_stream(SStr, "%Y-%m-%d %H:%M:%S.%f%Ez", TimePoint))
		{
			auto Duration = TimePoint.time_since_epoch();
			auto Duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(Duration).count();
			std::cout << "Milliseconds since epoch: " << Duration_ms << "\n";
		}
		//else {
		//	std::cerr << "Failed to parse\n";
		//}
		//DataRaw.resize(32);
		// [TBD] try to save the time
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	return utils::exit_code::EX_OK;
}
