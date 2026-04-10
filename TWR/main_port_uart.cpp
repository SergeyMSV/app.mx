#include "main.h"
#include "main_server.h"

#include "devDataSetConfig.h"

#include <algorithm>
#include <deque>
#include <iomanip>
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
	std::recursive_mutex m_ReceivedMtx;

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
	T GetReceived(std::size_t dataSizeMax)
	{
		std::lock_guard<std::recursive_mutex> lock(m_ReceivedMtx);
		if (m_Received.empty())
			return {};
		std::size_t Size = std::min(GetReceivedSize(), dataSizeMax);
		T Data;
		Data.reserve(Size);
		for (auto& i : m_Received)
		{
			if (Size >= i.size())
			{
				Data.insert(Data.end(), i.begin(), i.end());
				Size -= i.size();
				m_Received.pop_front();
			}
			else if (Size > 0)
			{
				Data.insert(Data.end(), i.begin(), i.begin() + Size);
				i.erase(i.begin() + Size, i.end());
				Size = 0;
			}

			if (!Size)
				break;
		}
		return Data;
	}

protected:
	std::size_t GetReceivedSize()
	{
		std::lock_guard<std::recursive_mutex> lock(m_ReceivedMtx);
		if (m_Received.empty())
			return 0;
		std::size_t Size = 0;
		for (auto& i : m_Received)
			Size += i.size();
		return Size;
	}

	void OnReceived(const std::vector<std::uint8_t>& data) override
	{
		std::lock_guard<std::recursive_mutex> lock(m_ReceivedMtx);
		m_Received.push_back(data);
		const std::size_t Size = GetReceivedSize();
		if (Size < dev::settings::port_uart::ReceivedSizeMax)
			return;
		std::size_t Remove = Size - dev::settings::port_uart::ReceivedSizeMax;
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
	}
};

static void ThreadUART(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server, int portIndex)
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

		tTWRQueueUARTCmd& QueueIn = TWRQueue.UART[portIndex]; // Port index is checked in config->GetUART(portIndex)

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
 					PTree.put("data", Data);
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

void ThreadUART0(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	ThreadUART(config, server, 0);
}

void ThreadUART1(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	ThreadUART(config, server, 1);
}

void ThreadUART2(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	ThreadUART(config, server, 2);
}

void ThreadUART3(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
{
	ThreadUART(config, server, 3);
}
