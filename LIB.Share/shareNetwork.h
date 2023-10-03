#pragma once

#include <utilsBase.h>

#include <array>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace share_network_udp
{

namespace asio_ip = boost::asio::ip;
constexpr std::size_t PacketSizeMax = 1024;

class UDP_Server
{
	asio_ip::udp::socket m_Socket;
	asio_ip::udp::endpoint m_EndpointRemote;
	std::array<char, PacketSizeMax> m_ReceiveBuffer;

public:
	UDP_Server(boost::asio::io_context& io_context, std::uint16_t port)
		: m_Socket(io_context, asio_ip::udp::endpoint(asio_ip::udp::v4(), port))
	{
		StartReceive();
	}

private:
	void StartReceive()
	{
		m_Socket.async_receive_from(
			boost::asio::buffer(m_ReceiveBuffer), m_EndpointRemote,
			boost::bind(&UDP_Server::OnReceived, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	virtual utils::tVectorUInt8 OnReceived(const utils::tVectorUInt8& data) = 0;
	virtual void OnSent(boost::shared_ptr<utils::tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;

	void OnReceived(const boost::system::error_code& error, std::size_t recvSize)
	{
		if (!error && recvSize)
		{
			boost::shared_ptr<utils::tVectorUInt8> Packet(new utils::tVectorUInt8(OnReceived({ m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + recvSize })));
			Send(Packet);
		}

		StartReceive();
	}

public:
	void Send(boost::shared_ptr<utils::tVectorUInt8> packet)
	{
		if (packet->empty())
			return;

		m_Socket.async_send_to(
			boost::asio::buffer(*packet), m_EndpointRemote,
			boost::bind(&UDP_Server::OnSent, this, packet, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
};

}
