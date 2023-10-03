#pragma once
#include <devConfig.h>

#include <string>
#include <vector>

#ifdef MXTWR_CLIENT
#include "sharePort.h"
#include <boost/asio.hpp>
namespace asio_ip = boost::asio::ip;
#endif // MXTWR_CLIENT

namespace share_port
{

class tSPI
#ifdef MXTWR_CLIENT
	: public tTWRClient
#endif // MXTWR_CLIENT
{
#if defined(MXTWR_CLIENT) || defined(MXTWR_SERVER)
	std::uint8_t m_Mode = 0;
	std::uint8_t m_Bits = 0;
	std::uint32_t m_Speed = 0; // Hz
#endif // MXTWR_CLIENT || MXTWR_SERVER
	std::uint16_t m_Delay = 0; // us

	int m_FileSPI = 0;
	std::string m_ErrMsg;

#ifdef MXTWR_CLIENT
	TWR::tEndpoint m_Endpoint = TWR::tEndpoint::DEMO;
#endif // MXTWR_CLIENT

public:
	tSPI() = delete;
#ifdef MXTWR_CLIENT
	tSPI(boost::asio::io_context& ioc, TWR::tEndpoint ep);
#else // MXTWR_CLIENT
	tSPI(const std::string& id, std::uint8_t mode, std::uint8_t bits, std::uint32_t speed_hz, std::uint16_t delay_us);
#endif // MXTWR_CLIENT
	tSPI(const tSPI&) = delete;
	~tSPI();

	bool IsReady() { return m_ErrMsg.empty(); }
	std::string GetErrMsg() { return m_ErrMsg; }
	
	std::uint8_t GetMode();
	bool SetMode(std::uint8_t val);
	std::uint8_t GetBits();
	bool SetBits(std::uint8_t val);
	std::uint32_t GetSpeed();
	bool SetSpeed(std::uint32_t val);
	std::uint16_t GetDelay() const { return m_Delay; }
	void SetDelay(std::uint16_t val) { m_Delay = val; }

	std::vector<std::uint8_t> Transaction(const std::vector<std::uint8_t>& tx);
};

}
