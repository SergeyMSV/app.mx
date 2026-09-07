#if defined(_WIN32)

#include <chrono>
#include <iostream> // [TEST]
#include <string>

namespace utils
{
namespace linux
{

std::string CmdLineWinTest(const std::string& cmd)
{
	if (cmd == "hwclock -f /dev/rtc1")
	{
		auto now = std::chrono::system_clock::now();
		return std::format("{:%Y-%m-%d %H:%M:%S+00:00}", now);
		//return "2026-07-13 17:36:44.751250+00:00";
	}
	if (cmd == "date -u +\"%Y-%m-%d %H:%M:%S.%6N\"")
	{
		auto now = std::chrono::system_clock::now();
		return std::format("{:%Y-%m-%d %H:%M:%S}", now);
	}
	if (cmd == "date -u +\"%Y-%m-%d %H:%M:%S\"")
	{
		auto now = std::chrono::system_clock::now();
		auto now_sec = floor<std::chrono::seconds>(now);
		return std::format("{:%Y-%m-%d %H:%M:%S}", now_sec);
	}
	if (cmd == "hwclock --rtc /dev/rtc1 -w --noadjfile --utc")
	{
		std::cout << "\nAdjust rtc1\n";
	}
	if (cmd == "journalctl -u systemd-timesyncd --grep \"Initial clock synchronization\" | grep \"UTC\"")
	{
		auto now = std::chrono::system_clock::now();
		// Требует C++20 + совместимую библиотеку
		std::cout << std::format("--> {:%Y-%m-%d %H:%M:%S}", now) << "\n";
		return "sdf";
		//return "Sep 06 21:40:58 opio systemd-timesyncd[167]: Initial clock synchronization to Sun 2026-09-06 21:40:58.513426 UTC.";
	}
	return {};
}

}
}

#endif // _WIN32
