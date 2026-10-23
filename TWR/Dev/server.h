#pragma once

#include <devConfig.h>

#include "types.h"

#include <shareNetwork.h>

#include <unordered_map>
#include <vector>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

using tTWRServerBase = share::network::udp::tUDPServerAsync<dev::settings::network_udp::PacketSizeMax>;

class tTWRServer : public tTWRServerBase
{
	std::unordered_map<std::uint16_t, std::vector<std::uint8_t>> m_ReceivedData;

public:
	tTWRServer(boost::asio::io_context& ioc, std::uint16_t port)
		:tTWRServerBase(ioc, port)
	{}

	void OnReceived(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& data) override;

	void SendResponse(const share::network::udp::tEndpoint& endpoint, boost::property_tree::ptree& pTree, const std::string& rsp);

private:
	void OnSent(boost::shared_ptr<std::vector<std::uint8_t>> packet, const boost::system::error_code& error, std::size_t bytes_transferred) override { }

	void HandlePacketBinary(const tPacketTWRCmdEp& cmd);
	bool HandlePacketJson(const share::network::udp::tEndpoint& endpoint, const std::vector<std::uint8_t>& cmd);
	bool PutInQueue(const tPacketTWRCmdEp& cmd);
};

}
