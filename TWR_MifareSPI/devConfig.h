#pragma once

#include <string_view>

#define MXTWR_CLIENT
#define MXTWR_PORT 58200

namespace dev
{
namespace settings
{
	constexpr char Version[] = "TWR.0.4.1";

	//constexpr std::string_view Host{ "127.0.0.1" };
	constexpr std::string_view Host{ "192.168.10.161" };
	//constexpr std::string_view Host{ "192.168.10.162" };

namespace network_udp
{
	constexpr std::size_t PacketSizeMax = 1024;
	constexpr std::size_t PacketDataSizeMax = 512;
}

namespace port_uart
{
	constexpr std::size_t ReceiveBufferSize = 1024;
	constexpr std::size_t ReceivedSizeMax = 4096;
}
}
}

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif // _WIN32
