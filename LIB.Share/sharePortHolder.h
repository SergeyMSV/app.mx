#pragma once
#include <devConfig.h>

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

	template <typename TConfig>
	explicit tPortHolder(TConfig& config)
		: m_Port(m_ioc, config)
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
