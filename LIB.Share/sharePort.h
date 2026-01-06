#pragma once
#include <devConfig.h>

#ifdef MXTWR_CLIENT

#include <atomic>
#include <condition_variable>
#include <string>
#include <thread>
#include <vector>

#include <utilsPacketTWR.h>

#include <boost/asio.hpp>
namespace asio_ip = boost::asio::ip;

namespace share
{
namespace port
{

using tTWREndpoint = utils::packet::twr::tEndpoint;
using tTWRChipControl = utils::packet::twr::tChipControl;
using tTWRPacketBase = utils::packet::twr::tPacketBase;
using tTWRPacketCmd = utils::packet::twr::tPacketCmd;
using tTWRPacketRsp = utils::packet::twr::tPacketRsp;

class tTWRClient
{
	asio_ip::udp::resolver m_Resolver;
	asio_ip::udp::endpoint m_ReceiverEndpoint;
	asio_ip::udp::socket m_Socket;

	enum class tState
	{
		None,
		Read,
		Write,
		Close,
	};
	std::atomic<tState> m_State = tState::None; // it's used by m_CtrlStateThread
	std::condition_variable m_cv;
	std::mutex m_cv_mtx;
	std::thread m_CtrlStateThread;

public:
	tTWRClient() = delete;
	explicit tTWRClient(boost::asio::io_context& ioc);
	tTWRClient(const tTWRClient&) = delete;
	tTWRClient(tTWRClient&&) = delete;
	~tTWRClient();
	tTWRClient& operator=(const tTWRClient&) = delete;
	tTWRClient& operator=(tTWRClient&&) = delete;

	void Transaction_SPI_Open(tTWREndpoint ep);
	void Transaction_SPI_Close(tTWREndpoint ep);
	std::vector<std::uint8_t> Transaction_SPI_Request(tTWREndpoint ep, const std::vector<std::uint8_t>& tx);
	void Transaction_SPI_SetChipControl(tTWREndpoint ep, tTWRChipControl tx);

private:
	tTWRPacketRsp Transaction(const tTWRPacketCmd& cmd);

	void SetState(tState state);
	void CtrlState();
	static void CtrlStateThread(void* obj);
};

}
}
#endif // MXTWR_CLIENT
