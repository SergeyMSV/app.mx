#pragma once

#include <utilsPortSerial.h>

namespace dev
{

using tPortUARTBase = utils::port::serial::tPortSerialAsync<>;

class tPortUART : public tPortUARTBase
{
	const std::string m_ID;

	std::vector<std::uint8_t> m_DataReceived;
	mutable std::mutex m_DataReceivedMtx;

public:
	tPortUART(boost::asio::io_context& ioc, const std::string& id, std::uint32_t baudRate)
		:m_ID(id), tPortUARTBase(ioc, id, baudRate)
	{
	}

	std::string GetID() const { return m_ID; }

	std::vector<std::uint8_t> GetReceived()
	{
		std::lock_guard<std::mutex> Lock(m_DataReceivedMtx);
		if (m_DataReceived.empty())
			return {};
		std::vector<std::uint8_t> Data = std::move(m_DataReceived);
		return Data;
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
