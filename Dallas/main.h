#pragma once
#include <devConfig.h>

#include <shareNetwork.h>

#include <utilsMultithread.h>

#include <string>

struct tPack
{
	share_network_udp::tEndpoint Endpoint;
	std::string Data;

	std::size_t size() { return Endpoint.size() + Data.size(); }
};

using tQueuePack = utils::multithread::tQueue<tPack, 1>;
