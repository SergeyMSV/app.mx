#pragma once

#ifdef _WIN32
#define GNSS_TEST
//#define LIB_SHARE_LOG_COUT
#define _WIN32_WINNT 0x0601
#endif // _WIN32

#include <cstdlib>

//#define MXTWR_CLIENT

#if defined(MXTWR_CLIENT)
#include <sharePort.h>
#define MXTWR_PORT 58200
#define MXTWR_EP_UART share::port::tTWREndpoint::UART1

namespace dev
{
namespace settings
{
	//constexpr std::string_view Host{ "127.0.0.1" };
	//constexpr char Host[] = "127.0.0.1";
	constexpr char Host[] = "192.168.10.162";

namespace network_udp
{
	constexpr std::size_t PacketSizeMax = 4096; // [TBD] it shall be equal to 1024
}
}
}
#endif // MXTWR_CLIENT

namespace dev
{
namespace settings
{
namespace port_uart
{
	constexpr std::size_t ReceiveBufferSize = 1024;
}
}
}

#include <utilsPacketNMEA.h>
using tPacketNMEA = utils::packet::nmea::tPacketNMEA_Common_CRC;
using tPacketNMEANoCRC = utils::packet::nmea::tPacketNMEA_Common_NoCRC;

#include <utilsLog.h>
#ifndef GNSS_TEST
using tLog = utils::log::tLogFile;
#else // GNSS_TEST
class tLog : public utils::log::tLogFile
{
public:
	tLog(const std::string& filesPath, std::uint32_t filesQtyMax, const std::string& filenamePrefix, std::uint32_t fileCapacity);
protected:
	void WriteLog(const std::string& a_text) override;
};
#endif // GNSS_TEST
