#pragma once
#include <devConfig.h>

#ifdef MXTWR_CLIENT

#include <string>
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

public:
	tTWRClient() = delete;
	tTWRClient(boost::asio::io_context& ioc);
	tTWRClient(const tTWRClient&) = delete;
	~tTWRClient();

	std::vector<std::uint8_t> Transaction_SPI_Request(TWR::tEndpoint ep, const std::vector<std::uint8_t>& tx);
	void Transaction_SPI_SetChipControl(TWR::tEndpoint ep, TWR::tChipControl tx);

private:
	tPacketTWRRsp Transaction(const tPacketTWRCmd& cmd);
};

}
#endif // MXTWR_CLIENT
