#pragma once

#include <array>
#include <boost/bind/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace share
{
namespace network
{
namespace udp
{

namespace asio_ip = boost::asio::ip;

using tEndpoint = asio_ip::udp::endpoint;

template <std::size_t PacketSizeMax>
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
		if (!m_Socket.is_open())
			return;
		m_Socket.async_receive_from(
			boost::asio::buffer(m_ReceiveBuffer), m_EndpointRemote,
			boost::bind(&tUDPServerAsync::OnReceivedAsync, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	virtual void OnReceived(const tEndpoint& endpoint, const std::vector<std::uint8_t>& data) = 0;
	virtual void OnSent(boost::shared_ptr<std::vector<std::uint8_t>> packet, const boost::system::error_code& error, std::size_t bytes_transferred) = 0;

	void OnReceivedAsync(const boost::system::error_code& error, std::size_t recvSize)
	{
		if (!error && recvSize)
			OnReceived(m_EndpointRemote, { m_ReceiveBuffer.begin(), m_ReceiveBuffer.begin() + recvSize });
		StartReceive();
	}

public:
	void Send(const tEndpoint& endpoint, boost::shared_ptr<std::vector<std::uint8_t>> packet)
	{
		if (!packet || packet->empty() || !m_Socket.is_open())
			return;
		m_Socket.async_send_to(
			boost::asio::buffer(*packet), endpoint,
			boost::bind(&tUDPServerAsync::OnSent, this, packet, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	void Send(const tEndpoint& endpoint, const std::vector<std::uint8_t>& packet)
	{
		auto Packet = boost::make_shared<std::vector<std::uint8_t>>(packet);
		Send(endpoint, Packet);
	}

	void Send(const tEndpoint& endpoint, const std::string& packet)
	{
		auto Packet = boost::make_shared<std::vector<std::uint8_t>>(packet.begin(), packet.end());
		Send(endpoint, Packet);
	}
};

}
}
}
