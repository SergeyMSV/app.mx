#pragma once

#include <utilsBase.h>

#include <array>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace share_network_udp
{

namespace asio_ip = boost::asio::ip;

using tEndpoint = asio_ip::udp::endpoint;

constexpr std::size_t PacketSizeMax = 1024;

class tUDPServerAsync
{
	asio_ip::udp::socket m_Socket;
	asio_ip::udp::endpoint m_EndpointRemote;
	std::array<char, PacketSizeMax> m_ReceiveBuffer;

public:
	tUDPServerAsync(boost::asio::io_context& ioc, std::uint16_t port)
		: m_Socket(ioc, asio_ip::udp::endpoint(asio_ip::udp::v4(), port))
	{
		StartReceive();
	}

private:
	void StartReceive()
	{
		m_Socket.async_receive_from(
			boost::asio::buffer(m_ReceiveBuffer), m_EndpointRemote,
			boost::bind(&tUDPServerAsync::OnReceivedAsync, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	virtual void OnReceived(const tEndpoint& endpoint, const utils::tVectorUInt8& data) = 0;
	virtual void OnSent(boost::shared_ptr<utils::tVectorUInt8> packet, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;

	void OnReceivedAsync(const boost::system::error_code& error, std::size_t recvSize)
	{
		if (!error && recvSize)
			OnReceived(m_EndpointRemote, { m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + recvSize });

		StartReceive();
	}

public:
	void Send(const tEndpoint& endpoint, boost::shared_ptr<utils::tVectorUInt8> packet)
	{
		if (!packet || packet->empty())
			return;

		m_Socket.async_send_to(
			boost::asio::buffer(*packet), endpoint,
			boost::bind(&tUDPServerAsync::OnSent, this, packet, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	void Send(const tEndpoint& endpoint, const utils::tVectorUInt8& packet)
	{
		boost::shared_ptr<utils::tVectorUInt8> Packet(new utils::tVectorUInt8(packet));
		Send(endpoint, Packet);
	}
};

}
