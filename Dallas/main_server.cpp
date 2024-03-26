#include "main_server.h"

#include <sstream>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

void tUDPServer::Send(const share_network_udp::tEndpoint& endpoint, const std::string& packet)
{
	utils::tVectorUInt8 Pack(packet.begin(), packet.end());
	share_network_udp::tUDPServerAsync::Send(endpoint, Pack);
}

void tUDPServer::OnReceived(const share_network_udp::tEndpoint& endpoint, const tVectorUInt8& data)
{
	if (data.empty())
		return;

	std::shared_ptr<tQueuePack> QueueIn = m_QueueInWeak.lock();
	if (!QueueIn)
		return;

	std::string DataStr(data.begin(), data.end());
	std::stringstream SStr(std::move(DataStr));

	boost::property_tree::ptree PTree;
	try
	{
		boost::property_tree::json_parser::read_json(SStr, PTree);
	}
	catch (...)
	{
		return; // If received data is not comply with JSON, it'll be dropped with no response.
	}

	QueueIn->push_back({ endpoint, SStr.str() });
}
