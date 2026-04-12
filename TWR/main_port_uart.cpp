#include "main.h"
#include "main_server.h"

#include "devDataSetConfig.h"

#include <algorithm>
#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>

#include <utilsPortSerial.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using tUARTBase = utils::port::serial::tPortSerialAsync<dev::settings::port_uart::ReceiveBufferSize>;

class tUART : public tUARTBase
{
	share::network::udp::tEndpoint m_EndpointLast;
	std::deque<std::vector<std::uint8_t>> m_Received;
	std::size_t m_ReceivedSize = 0;
	std::mutex m_ReceivedMtx;

public:
	tUART() = delete;
	tUART(boost::asio::io_context& ioc, const share::config::port::tUART_Config& config)
		:tUARTBase(ioc, config.ID, config.BR)
	{
	}

	void SetEndpoint(share::network::udp::tEndpoint endpoint)
	{
		m_EndpointLast = endpoint;
	}

	template <typename T>
	T GetReceived(std::size_t dataSize)
	{
		std::lock_guard<std::mutex> lock(m_ReceivedMtx);
		if (m_Received.empty())
			return {};
#ifdef TWR_DEBUG
		assert(CalcReceivedSize_NoLock() == m_ReceivedSize);
#endif // TWR_DEBUG
		auto CorrReceivedSize = [this](std::size_t diff)
			{
				if (m_ReceivedSize < diff)
				{
					std::cerr << "m_ReceivedSize is wrong: " << std::to_string(m_ReceivedSize) << '\n';
					CalcReceivedSize_NoLock();
					return;
				}
				m_ReceivedSize -= diff;
			};
		T Data;
		Data.reserve(dataSize);
		for (auto& i : m_Received)
		{
			if (dataSize >= i.size())
			{
				Data.insert(Data.end(), i.begin(), i.end());
				dataSize -= i.size();
				CorrReceivedSize(i.size());
				m_Received.pop_front(); // [*] 'i' is not valid any more
				if (!dataSize)
					break;
			}
			else if (dataSize > 0)
			{
				Data.insert(Data.end(), i.begin(), i.begin() + dataSize);
				i.erase(i.begin(), i.begin() + dataSize);
				CorrReceivedSize(dataSize);
				break;
			}
		}
#ifdef TWR_DEBUG
		assert(CalcReceivedSize_NoLock() == m_ReceivedSize);
#endif // TWR_DEBUG
		return Data;
	}

protected:
	void OnReceived(std::vector<std::uint8_t>& data) override
	{
		if (data.empty())
			return;
		std::lock_guard<std::mutex> lock(m_ReceivedMtx);
		m_Received.push_back(std::move(data));
		m_ReceivedSize += m_Received.back().size();
		if (m_ReceivedSize < dev::settings::port_uart::ReceivedSizeMax)
			return;
		std::size_t Remove = m_ReceivedSize - dev::settings::port_uart::ReceivedSizeMax;
		while (Remove)
		{
			if (m_Received.size() == 1) // Last part shall be kept even if it bigger than the limit (dev::settings::port_uart::ReceivedSizeMax).
				break;
			const std::size_t PartSize = m_Received.front().size();
			m_Received.pop_front();
			if (PartSize > Remove)
				break;
			Remove -= PartSize;
		}
		m_ReceivedSize = CalcReceivedSize_NoLock();
	}

private:
	std::size_t CalcReceivedSize_NoLock() const
	{
		std::size_t Size = 0;
		for (auto& i : m_Received)
			Size += i.size();
		return Size;
	}
};

static void ThreadUART_JSON(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server, int portIndex)
{
	using tPortHld = tPortHolder<tUART>;

	try
	{
		std::weak_ptr<dev::tDataSetConfig> ConfigWeak(config);

		share::config::port::tUART_Config PortConfig{};
		{
			std::shared_ptr<dev::tDataSetConfig> Config = ConfigWeak.lock();
			PortConfig = config->GetUART(portIndex);
			if (PortConfig.IsWrong())
				return; // [TBD] throw an exception
		}

		tTWRQueueUARTJSONCmd& QueueIn = TWRQueue.UART_JSON[portIndex]; // Port index is checked in config->GetUART(portIndex)

		std::unique_ptr<tPortHld> PortPtr;

		while (!ConfigWeak.expired())
		{
			if (QueueIn.empty())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(!PortPtr ? 100 : 1));
				continue;
			}

			tPack Pack = QueueIn.get_front();
			std::stringstream SStr(std::move(Pack.Value));

			try
			{
				boost::property_tree::ptree PTree;
				boost::property_tree::json_parser::read_json(SStr, PTree);
				std::string Cmd = PTree.get<std::string>("cmd");

				if (Cmd == "open")
				{
					if (PortPtr)
						PortPtr.reset();
					PortPtr = std::make_unique<tPortHld>(PortConfig);
					server.SendResponse(Pack.Endpoint, PTree, "ok");
					continue;
				}
				else if (Cmd == "close") // It's just a response, nothing more (for compatibility reasons).
				{
					PortPtr.reset();
					server.SendResponse(Pack.Endpoint, PTree, "ok");
					continue;
				}

				if (!PortPtr)
					continue;

				if (Cmd == "receive")
				{
					std::string Data = (*PortPtr)().GetReceived<std::string>(dev::settings::network_udp::PacketDataSizeMax); // [TBD] PacketSizeMax vs. PacketDataSizeMax
					std::replace_if(Data.begin(), Data.end(), [](char c) { return c < 0x20 && c != 0x0a && c != 0x0d; }, '.');
 					PTree.put("data", std::move(Data));
					server.SendResponse(Pack.Endpoint, PTree, "ok");
				}
				else if (Cmd == "send")
				{
					std::string DataStr = PTree.get<std::string>("data");
					PTree.erase("data");
					(*PortPtr)().Send(std::vector<std::uint8_t>(DataStr.begin(), DataStr.end()));
					server.SendResponse(Pack.Endpoint, PTree, "ok");
				}
				else
				{
					server.SendResponse(Pack.Endpoint, PTree, "unknown");
				}
			}
			catch (const boost::system::system_error& ex)
			{
				PortPtr.reset();
				std::cerr << ex.what() << '\n';
			}
			catch (...) {} // Format JSON shall be verified in the receiver (e.g. in UDP-Server)
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

void ThreadUART0_JSON(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	ThreadUART_JSON(config, server, 0);
}

void ThreadUART1_JSON(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	ThreadUART_JSON(config, server, 1);
}

void ThreadUART2_JSON(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	ThreadUART_JSON(config, server, 2);
}

void ThreadUART3_JSON(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	ThreadUART_JSON(config, server, 3);
}
