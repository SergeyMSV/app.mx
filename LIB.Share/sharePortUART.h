#pragma once
#include <devConfig.h>

#ifdef MXTWR_CLIENT

#include <string>
#include <vector>

#include "sharePort.h"
#include <boost/asio.hpp>
namespace asio_ip = boost::asio::ip;

namespace share
{
namespace port
{

class tUART : public tTWRClient
{
public:
	tUART(boost::asio::io_context& ioc, const std::string& id, std::uint32_t baudRate);
#else // MXTWR_CLIENT
	tUART(const std::string& id, std::uint8_t mode, std::uint8_t bits, std::uint32_t speed_hz, std::uint16_t delay_us);

	tUART(const tUART&) = delete;
	~tUART();

	bool IsReady() const { return m_ErrMsg.empty(); }
	std::string GetErrMsg() const { return m_ErrMsg; }
	
	//std::uint8_t GetMode();
	//bool SetMode(std::uint8_t val);
	//std::uint8_t GetBits();
	//bool SetBits(std::uint8_t val);
	//std::uint32_t GetSpeed();
	//bool SetSpeed(std::uint32_t val);
	//std::uint16_t GetDelay() const { return m_Delay; }
	//void SetDelay(std::uint16_t val) { m_Delay = val; }

	//std::vector<std::uint8_t> Transaction(const std::vector<std::uint8_t>& tx);
};

}
}
#endif // MXTWR_CLIENT
