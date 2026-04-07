#pragma once

#include <devConfig.h>

#include <utilsPacketTWR.h>

#include <shareNetwork.h>

#include <utilsMultithread.h>

using tTWRMsgId = utils::packet::twr::tMsgId;
using tTWRMsgStatus = utils::packet::twr::tMsgStatus;
using tTWREndpoint = utils::packet::twr::tEndpoint;
using tTWRChipControl = utils::packet::twr::tChipControl;
using tTWRSPIPortSettings = utils::packet::twr::tSPIPortSettings;
using tTWRPacketBase = utils::packet::twr::tPacketBase;
using tTWRPacketCmd = utils::packet::twr::tPacketCmd;
using tTWRPacketRsp = utils::packet::twr::tPacketRsp;

template<class T>
struct tPack
{
	share::network::udp::tEndpoint Endpoint;
	T Value;
};

using tPacketTWRCmdEp = tPack<tTWRPacketCmd>;
using tTWRQueueDEMOCmd = utils::multithread::tQueue<tPacketTWRCmdEp, 2>;
using tTWRQueueSPICmd = utils::multithread::tQueue<tPacketTWRCmdEp, 2>;

using tPacketTWRDALLASCmdEp = tPack<std::string>;
using tTWRQueueDALLASCmd = utils::multithread::tQueue<tPacketTWRDALLASCmdEp, 1>;

using tPacketTWRUARTCmdEp = tPack<std::string>;
using tTWRQueueUARTCmd = utils::multithread::tQueue<tPacketTWRUARTCmdEp, 1>;

struct tTWRQueue
{
	tTWRQueueDEMOCmd DEMO;
	tTWRQueueSPICmd SPI0_CS0;
	tTWRQueueDALLASCmd DALLAS;
	tTWRQueueUARTCmd UART[4];
};

extern tTWRQueue TWRQueue;
