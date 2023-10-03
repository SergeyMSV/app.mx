#pragma once
#include <devConfig.h>

#include <utilsPacketTWR.h>

#include <shareNetwork.h>

#include <chrono>
#include <deque>
#include <mutex>
#include <thread>

namespace asio_ip = boost::asio::ip;
namespace TWR = utils::packet_TWR;

using tPacketTWR = utils::packet_TWR::tPacketTWR;
using tPacketTWRCmd = utils::packet_TWR::tPacketTWRCmd;
using tPacketTWRRsp = utils::packet_TWR::tPacketTWRRsp;

template <class T, std::size_t Size>
class tTWRQueueEntity
{
	std::deque<T> m_Queue;
	std::mutex m_QueueMtx;

public:
	T front()
	{
		std::lock_guard<std::mutex> guard(m_QueueMtx);
		T Pack = m_Queue.front();
		m_Queue.pop_front();
		return Pack;
	}
	void push_back(const T& val)
	{
		std::lock_guard<std::mutex> guard(m_QueueMtx);
		if (m_Queue.size() >= Size)
			m_Queue.pop_front();
		m_Queue.push_back(val);
	}
	void clear()
	{
		std::lock_guard<std::mutex> guard(m_QueueMtx);
		m_Queue.clear();
	}
	bool empty()
	{
		std::lock_guard<std::mutex> guard(m_QueueMtx);
		return m_Queue.empty();
	}
};

using tTWRQueueDEMOCmd = tTWRQueueEntity<tPacketTWRCmd, 2>;
using tTWRQueueSPICmd = tTWRQueueEntity<tPacketTWRCmd, 2>;

struct tTWRQueue
{
	tTWRQueueDEMOCmd DEMO;
	tTWRQueueSPICmd SPI0_CS0;
};

extern tTWRQueue TWRQueue;
