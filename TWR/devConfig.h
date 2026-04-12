#pragma once

#include <cstdlib>

namespace dev
{
namespace settings
{
	constexpr char Version[] = "TWR.0.3.3";

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

#define MXTWR_SERVER

#ifdef _WIN32
#define UDP_SERVER_TEST
#define TWR_DEBUG
#define _WIN32_WINNT 0x0601
#endif // _WIN32