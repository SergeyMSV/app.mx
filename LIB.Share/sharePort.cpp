#include "sharePort.h"
#include "shareNetwork.h"

#ifdef MXTWR_CLIENT

namespace share_port
{

tTWRClient::tTWRClient(boost::asio::io_context& ioc)
	:m_Resolver(asio_ip::udp::resolver(ioc)), m_Socket(asio_ip::udp::socket(ioc)),
	m_State(tState::None), m_CtrlStateThread(&tTWRClient::CtrlStateThread, this)
{
	m_ReceiverEndpoint = *m_Resolver.resolve(asio_ip::udp::v4(), Host, std::to_string(MXTWR_PORT)).begin();
	m_Socket.open(asio_ip::udp::v4());
}

tTWRClient::~tTWRClient()
{
	m_State = tState::Close;
	m_cv.notify_all();
	m_CtrlStateThread.join();

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
	SetState(tState::Write);

	tVectorUInt8 Pack = cmd.ToVector();
	m_Socket.send_to(boost::asio::buffer(Pack.data(), Pack.size()), m_ReceiverEndpoint);

	SetState(tState::Read);

	std::array<char, share_network_udp::PacketSizeMax> ReceiveBuffer;
	asio_ip::udp::endpoint SenderEndpoint;
	std::size_t Size = m_Socket.receive_from(boost::asio::buffer(ReceiveBuffer), SenderEndpoint);

	SetState(tState::None);

	if (!Size)
		return {};
	tVectorUInt8 ReceivedData(ReceiveBuffer.begin(), ReceiveBuffer.begin() + Size);
	tPacketTWRRsp Rsp;
	std::size_t PackSize = tPacketTWRRsp::Find(ReceivedData, Rsp);
	if (!PackSize)
		return {};
	return Rsp;
}

void tTWRClient::SetState(tState state)
{
	m_State = state;
	m_cv.notify_all();
}

void tTWRClient::CtrlState()
{
	while (m_State != tState::Close)
	{
		switch (m_State)
		{
		case  tState::None:
		{
			std::unique_lock<std::mutex> Lock(m_cv_mtx);
			m_cv.wait(Lock);
			break;
		}
		case tState::Read:
		case tState::Write:
		{
			std::unique_lock<std::mutex> Lock(m_cv_mtx);
			if (m_cv.wait_for(Lock, std::chrono::seconds(1)) == std::cv_status::timeout) //[#] < 1 s. That is available period of time to send/receive.
			{
				// It breaks the receiving process through blocking socket (receive_from) if no data can be received for any reason.
				m_Socket.shutdown(boost::asio::socket_base::shutdown_both);
				m_Socket.close();
			}
			break;
		}
		}
	}
}

void tTWRClient::CtrlStateThread(void* obj)
{
	auto TWRClient = static_cast<tTWRClient*>(obj);
	TWRClient->CtrlState();
}

}
#endif // MXTWR_CLIENT
