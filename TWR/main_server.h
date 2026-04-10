#pragma once

#include "main.h"
#include <unordered_map>
#include <vector>

#include <boost/property_tree/ptree.hpp>

class tTWRServer : public share::network::udp::tUDPServerAsync
{
	std::unordered_map<std::uint16_t, std::vector<std::uint8_t>> m_ReceivedData;

public:
	tTWRServer(boost::asio::io_context& ioc, std::uint16_t port)
		:tUDPServerAsync(ioc, port)
	{}

	void OnReceived(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& data) override;

	void SendResponse(const share::network::udp::tEndpoint& endpoint, boost::property_tree::ptree& pTree, const std::string& rsp);

private:
	void OnSent(boost::shared_ptr<std::vector<std::uint8_t>> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override { }

	void HandlePacketBinary(const tPacketTWRCmdEp& cmd);
	bool HandlePacketJson(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& cmd);
	bool PutInQueue(const tPacketTWRCmdEp& cmd);
};

template <class T>
class tPortHolder
{
	boost::asio::io_context m_ioc;
	std::thread m_iocThread;
	T m_Port;

public:
	tPortHolder() = delete;

	template <typename TArg1, typename TArg2>
	tPortHolder(const TArg1& arg1, const TArg2& arg2)
		:m_Port(m_ioc, arg1, arg2)
	{
		m_iocThread = std::thread([this]() { m_ioc.run(); });
	}

	~tPortHolder()
	{
		m_Port.Close();
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // It's put here in order to avoid exceptions when the port (serial) is restarted.
		m_ioc.stop();
		m_iocThread.join();
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // It's put here in order to avoid exceptions when the port (serial) is restarted (it happens from time to time).
	}

	T& operator()() { return m_Port; }
};
