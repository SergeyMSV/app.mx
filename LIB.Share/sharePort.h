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

namespace share_port
{

namespace TWR = utils::packet_TWR;
using tPacketTWR = utils::packet_TWR::tPacketTWR;
using tPacketTWRCmd = utils::packet_TWR::tPacketTWRCmd;
using tPacketTWRRsp = utils::packet_TWR::tPacketTWRRsp;

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

	std::vector<std::uint8_t> Transaction_SPI_Request(TWR::tEndpoint ep, const std::vector<std::uint8_t>& tx);
	void Transaction_SPI_SetChipControl(TWR::tEndpoint ep, TWR::tChipControl tx);

private:
	tPacketTWRRsp Transaction(const tPacketTWRCmd& cmd);

	void SetState(tState state);
	void CtrlState();
	static void CtrlStateThread(void* obj);
};

}
#endif // MXTWR_CLIENT