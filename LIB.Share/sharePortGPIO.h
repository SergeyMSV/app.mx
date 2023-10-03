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

class tGPIO
#ifdef MXTWR_CLIENT
	: public tTWRClient
#endif // MXTWR_CLIENT
{
	std::string m_ID;
#if defined(MXTWR_CLIENT) || defined(MXTWR_SERVER)
	bool m_State = false;
#endif // MXTWR_CLIENT || MXTWR_SERVER

	std::string m_ErrMsg;

#ifdef MXTWR_CLIENT
	TWR::tEndpoint m_Endpoint = TWR::tEndpoint::DEMO;
#endif // MXTWR_CLIENT

public:
	tGPIO() = delete;
#ifdef MXTWR_CLIENT
	tGPIO(boost::asio::io_context& ioc, TWR::tEndpoint ep);
#else // MXTWR_CLIENT
	tGPIO(const std::string& id);
#endif // MXTWR_CLIENT
	tGPIO(const tGPIO&) = delete;
	~tGPIO();

	bool IsReady() { return m_ErrMsg.empty(); }
	std::string GetErrMsg() { return m_ErrMsg; }

	bool GetState();
	void SetState(bool state);
};

}
