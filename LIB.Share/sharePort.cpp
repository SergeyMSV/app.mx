#include "sharePort.h"
#include "shareNetwork.h"

#ifdef MXTWR_CLIENT

namespace share_port
{

tTWRClient::tTWRClient(boost::asio::io_context& ioc)
	:m_Resolver(asio_ip::udp::resolver(ioc)), m_Socket(asio_ip::udp::socket(ioc))
{
	m_ReceiverEndpoint = *m_Resolver.resolve(asio_ip::udp::v4(), Host, std::to_string(MXTWR_PORT)).begin();
	m_Socket.open(asio_ip::udp::v4());
}

tTWRClient::~tTWRClient()
{
	m_Socket.close();
}

std::vector<std::uint8_t> tTWRClient::Transaction_SPI_Request(TWR::tEndpoint ep, const std::vector<std::uint8_t>& tx)
{
	return Transaction(tPacketTWRCmd::Make_SPI_Request(ep, tx)).GetPayload();
}

void tTWRClient::Transaction_SPI_SetChipControl(TWR::tEndpoint ep, TWR::tChipControl tx)
{
	Transaction(tPacketTWRCmd::Make_SPI_SetChipControl(ep, tx)).GetPayload();
}

tPacketTWRRsp tTWRClient::Transaction(const tPacketTWRCmd& cmd)
{
	tVectorUInt8 Pack = cmd.ToVector();
	m_Socket.send_to(boost::asio::buffer(Pack.data(), Pack.size()), m_ReceiverEndpoint);

	std::array<char, share_network_udp::PacketSizeMax> ReceiveBuffer;
	asio_ip::udp::endpoint SenderEndpoint;
	std::size_t Size = m_Socket.receive_from(boost::asio::buffer(ReceiveBuffer), SenderEndpoint);
	if (!Size)
		return {};
	tVectorUInt8 ReceivedData(ReceiveBuffer.begin(), ReceiveBuffer.begin() + Size);
	tPacketTWRRsp Rsp;
	std::size_t PackSize = tPacketTWRRsp::Find(ReceivedData, Rsp);
	if (!PackSize)
		return {};
	return Rsp;
}

}
#endif // MXTWR_CLIENT
