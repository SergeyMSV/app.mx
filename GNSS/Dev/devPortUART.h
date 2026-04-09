#pragma once

#if defined(MXTWR_CLIENT)
#include <sharePortUART.h>
using tPortUARTBase = share::port::tUART_JSON;
#else
#include <utilsPortSerial.h>
using tPortUARTBase = utils::port::serial::tPortSerialAsync<>;
#endif

namespace dev
{

class tPortUART : public tPortUARTBase
{
	const std::string m_ID;

	std::vector<std::uint8_t> m_DataReceived;
	mutable std::mutex m_DataReceivedMtx;

public:
	tPortUART(boost::asio::io_context& ioc, const std::string& id, std::uint32_t baudRate)
		:tPortUARTBase(ioc, id, baudRate), m_ID(id)
	{
	}

	std::string GetID() const { return m_ID; }

	std::vector<std::uint8_t> GetReceived()
	{
		std::lock_guard<std::mutex> Lock(m_DataReceivedMtx);
		if (m_DataReceived.empty())
			return {};
		return std::move(m_DataReceived);
	}

	bool Send(const std::vector<std::uint8_t>& data)
	{
		return tPortUARTBase::Send(data);
	}

	bool Send(const std::string& data)
	{
		return tPortUARTBase::Send(std::vector<std::uint8_t>(data.begin(), data.end()));
	}

protected:
	void OnReceived(const std::vector<std::uint8_t>& data) override
	{
		std::lock_guard<std::mutex> Lock(m_DataReceivedMtx);
		m_DataReceived.insert(m_DataReceived.end(), data.begin(), data.end());
	}
};

}
