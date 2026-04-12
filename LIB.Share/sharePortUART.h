#pragma once
#include <devConfig.h>

#ifdef MXTWR_CLIENT

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "sharePort.h"
#include <boost/asio.hpp>
namespace asio_ip = boost::asio::ip;

namespace share
{
namespace port
{

class tUART_JSON : public tTWRClient
{
	std::thread m_ThreadReceive;
	std::atomic<bool> m_ThreadReceiveExit = false;

	std::uint32_t m_BaudRate; // [TBD] it is to be on the other side. THAT'S NOT CORRECT.

public:
	tUART_JSON(boost::asio::io_context& ioc, const std::string& id, std::uint32_t baudRate)
		:tTWRClient(ioc), m_BaudRate(baudRate)
	{
		TransactionJSON_UART_Open(MXTWR_EP_UART);

		// [TBD] set baudrate

		m_ThreadReceive = std::thread([this]() { Receiving(); });
	}

	tUART_JSON(const tUART_JSON&) = delete;
	~tUART_JSON()
	{
		m_ThreadReceiveExit = true;
		m_ThreadReceive.join();
		TransactionJSON_UART_Close(MXTWR_EP_UART);
	}

	bool Send(const std::vector<std::uint8_t>& data)
	{
		if (data.empty())
			return false;
		TransactionJSON_UART_Send(MXTWR_EP_UART, data);
		return true;
	}

	bool Send(const std::string& data)
	{
		if (data.empty())
			return false;
		TransactionJSON_UART_Send(MXTWR_EP_UART, data);
		return true;
	}

	std::uint32_t GetBaudRate() const
	{
		//if (!m_Port.is_open())
		//	return 0;
		//boost::asio::serial_port_base::baud_rate Br{};
		//boost::system::error_code Ec{};
		//m_Port.get_option(Br, Ec);
		//if (Ec)
		//	return 0;
		//return Br.value();
		return m_BaudRate;
	}

	void SetBaudRate(std::uint32_t val)
	{
		//std::cout << "SetBR : " << std::to_string(val) << '\n';
		//if (!m_Port.is_open())
		//	return;
		//m_Port.set_option(boost::asio::serial_port_base::baud_rate(val));
	}

protected:
	virtual void OnReceived(std::vector<std::uint8_t>& data) = 0;

	void Receiving()
	{
		while (!m_ThreadReceiveExit)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); // [#]
			std::vector<std::uint8_t> Received = TransactionJSON_UART_Receive(MXTWR_EP_UART);
			if (!Received.empty())
				OnReceived(Received);
		}
	}

	//bool IsReady() const { return m_ErrMsg.empty(); }
	//std::string GetErrMsg() const { return m_ErrMsg; }
	
	//std::uint8_t GetMode();
	//bool SetMode(std::uint8_t val);
	//std::uint8_t GetBits();
	//bool SetBits(std::uint8_t val);
	//std::uint32_t GetSpeed();
	//bool SetSpeed(std::uint32_t val);
	//std::uint16_t GetDelay() const { return m_Delay; }
	//void SetDelay(std::uint16_t val) { m_Delay = val; }
};

class tUART : public tTWRClient
{
public:
	tUART(boost::asio::io_context& ioc, const std::string& id, std::uint32_t baudRate)
		:tTWRClient(ioc)
	{
		Transaction_UART_Open(MXTWR_EP_UART);
		// set baudrate
	}

	tUART(const tUART&) = delete;
	~tUART()
	{
		Transaction_UART_Close(MXTWR_EP_UART);
	}

	bool Send(const std::vector<std::uint8_t>& data)
	{
		if (data.empty())
			return false;

		// [TBD] Something shall limit incoming data for sending here.
	//
	//	std::lock_guard<std::mutex> Lock(m_Mtx);
	//
	//	bool StartSending = m_DataSent.empty();
	//
	//	m_DataSent.push(data);
	//
	//	if (StartSending)
	//		Send();

		return true;
	}

	std::uint32_t GetBaudRate() const
	{
		//if (!m_Port.is_open())
		//	return 0;
		//boost::asio::serial_port_base::baud_rate Br{};
		//boost::system::error_code Ec{};
		//m_Port.get_option(Br, Ec);
		//if (Ec)
		//	return 0;
		//return Br.value();
		return 0;
	}

	void SetBaudRate(std::uint32_t val)
	{
		//std::cout << "SetBR : " << std::to_string(val) << '\n';
		//if (!m_Port.is_open())
		//	return;
		//m_Port.set_option(boost::asio::serial_port_base::baud_rate(val));
	}

protected:
	virtual void OnReceived(const std::vector<std::uint8_t>& data) = 0;

	//bool IsReady() const { return m_ErrMsg.empty(); }
	//std::string GetErrMsg() const { return m_ErrMsg; }
	
	//std::uint8_t GetMode();
	//bool SetMode(std::uint8_t val);
	//std::uint8_t GetBits();
	//bool SetBits(std::uint8_t val);
	//std::uint32_t GetSpeed();
	//bool SetSpeed(std::uint32_t val);
	//std::uint16_t GetDelay() const { return m_Delay; }
	//void SetDelay(std::uint16_t val) { m_Delay = val; }

	//std::vector<std::uint8_t> Transaction(const std::vector<std::uint8_t>& tx);
};

}
}
#endif // MXTWR_CLIENT
