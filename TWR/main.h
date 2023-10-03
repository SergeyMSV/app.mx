#pragma once
#include <devConfig.h>

#include <utilsPacketTWR.h>

#include <shareNetwork.h>

#include <utilsMultithread.h>

#include <thread>

namespace asio_ip = boost::asio::ip;
namespace TWR = utils::packet_TWR;

using tPacketTWR = utils::packet_TWR::tPacketTWR;
using tPacketTWRCmd = utils::packet_TWR::tPacketTWRCmd;
using tPacketTWRRsp = utils::packet_TWR::tPacketTWRRsp;

template<class T>
struct tPack
{
	share_network_udp::tEndpoint Endpoint;
	T Value;
};

using tPacketTWRCmdEp = tPack<tPacketTWRCmd>;

using tTWRQueueDEMOCmd = utils::multithread::tQueue<tPacketTWRCmdEp, 2>;
using tTWRQueueSPICmd = utils::multithread::tQueue<tPacketTWRCmdEp, 2>;

struct tTWRQueue
{
	tTWRQueueDEMOCmd DEMO;
	tTWRQueueSPICmd SPI0_CS0;
};

extern tTWRQueue TWRQueue;
