#pragma once
#include <devConfig.h>

#include <shareNetwork.h>

#include <deque>
#include <mutex>
#include <string>

namespace utils
{
namespace multithread
{

template <class T, std::size_t Size> // [TBD] put in utils::multithread
class tQueue
{
	std::deque<T> m_Queue;
	std::mutex m_QueueMtx;

public:
	T get_front()
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

}
}

struct tPack
{
	share_network_udp::tEndpoint Endpoint;
	std::string Data;

	std::size_t size() { return Endpoint.size() + Data.size(); }
};

using tQueuePack = utils::multithread::tQueue<tPack, 1>;
