#pragma once

constexpr char g_FileNameTempPrefix[] = "_";

#define MXTWR_CLIENT

#if defined(MXTWR_CLIENT)
#include <cstdint>
#include <sharePort.h>
//#include <utilsPacketTWR.h>
#define MXTWR_PORT 58200
#define MXTWR_EP_UART share::port::tTWREndpoint::UART1
#endif

namespace settings
{
	//constexpr std::string_view Host{ "127.0.0.1" };
	//constexpr char Host[] = "127.0.0.1";
	constexpr char Host[] = "192.168.10.162";
}

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif // _WIN32

#include <utilsPacket.h>
#include <utilsPacketNMEA.h>

using tPacketNMEA = utils::packet::nmea::tPacketNMEA_Common_CRC;
using tPacketNMEANoCRC = utils::packet::nmea::tPacketNMEA_Common_NoCRC;

//#define GNSS_VALUE_INVALID_LAT 999999
//#define GNSS_VALUE_INVALID_LON 999999
