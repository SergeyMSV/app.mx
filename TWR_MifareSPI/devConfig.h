#pragma once

#include <string_view>

#define MXTWR_CLIENT
#define MXTWR_PORT 58200

namespace settings
{
	//constexpr std::string_view Host{ "127.0.0.1" };
	constexpr std::string_view Host{ "192.168.10.161" };
}

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif // _WIN32