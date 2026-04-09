#include "sharePort.h"

#ifdef MXTWR_CLIENT

#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace share
{
namespace port
{

tTWRClient::tTWRClient(boost::asio::io_context& ioc)
	:m_Resolver(asio_ip::udp::resolver(ioc)), m_Socket(asio_ip::udp::socket(ioc)),
	m_State(tState::None), m_CtrlStateThread(&tTWRClient::CtrlStateThread, this)
{
	m_ReceiverEndpoint = *m_Resolver.resolve(asio_ip::udp::v4(), settings::Host, std::to_string(MXTWR_PORT)).begin();
	m_Socket.open(asio_ip::udp::v4());

	// [TBD] check TWR Version
	//std::string Rsp = TransactionJSON(MakeCmdJSON(tTWREndpoint::Control, "version")); //"{\"ep\":\"control\",\"cmd\":\"version\"}");
}

tTWRClient::~tTWRClient()
{
	m_State = tState::Close;
	m_cv.notify_all();
	m_CtrlStateThread.join();

	m_Socket.close();
}

void tTWRClient::Transaction_SPI_Open(tTWREndpoint ep)
{
	Transaction(tTWRPacketCmd::Make_SPI_Open(ep));
}

void tTWRClient::Transaction_SPI_Close(tTWREndpoint ep)
{
	Transaction(tTWRPacketCmd::Make_SPI_Close(ep));
}

std::vector<std::uint8_t> tTWRClient::Transaction_SPI_Request(tTWREndpoint ep, const std::vector<std::uint8_t>& tx)
{
	return Transaction(tTWRPacketCmd::Make_SPI_Request(ep, tx)).GetPayload();
}

void tTWRClient::Transaction_SPI_SetChipControl(tTWREndpoint ep, tTWRChipControl tx)
{
	Transaction(tTWRPacketCmd::Make_SPI_SetChipControl(ep, tx));
}

void tTWRClient::Transaction_UART_Open(tTWREndpoint ep)
{
	Transaction(tTWRPacketCmd::Make_UART_Open(ep));
}

void tTWRClient::Transaction_UART_Close(tTWREndpoint ep)
{
	Transaction(tTWRPacketCmd::Make_UART_Close(ep));
}

std::vector<std::uint8_t> tTWRClient::Transaction_UART_Receive(tTWREndpoint ep)
{
	return Transaction(tTWRPacketCmd::Make_UART_Receive(ep)).GetPayload();
}

void tTWRClient::Transaction_UART_Send(tTWREndpoint ep, const std::vector<std::uint8_t>& data)
{
	Transaction(tTWRPacketCmd::Make_UART_Send(ep, data));
}

bool tTWRClient::TransactionJSON_UART_Open(tTWREndpoint ep)
{
	std::stringstream SStr;
	SStr << TransactionJSON(MakeCmdJSON(ep, "open"));
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(SStr, PTree);
	std::string Ans = PTree.get<std::string>("rsp");
	return Ans == "ok";
}

void tTWRClient::TransactionJSON_UART_Close(tTWREndpoint ep)
{
	TransactionJSON(MakeCmdJSON(ep, "close"));
}

std::vector<std::uint8_t> tTWRClient::TransactionJSON_UART_Receive(tTWREndpoint ep)
{
	std::stringstream SStr;
	std::string Str = TransactionJSON(MakeCmdJSON(ep, "receive"));


	//Str = "{\"ep\": \"\uart_1\",\"cmd\" : \"receive\",\"data\" : \"""VЦa’GPRMC,184649.022,V,5556.8002,N,03737.2609,E,9999.99,999.99,090426,,*26\\r\\n$GPVTG,999.99,T,,M,\",\"rsp\" : \"ok\"}";

	//Str = "{\"ep\": \"\uart_1\",\"cmd\" : \"receive\",\"data\" : \"""Vж\\u00199GPRMC,184649.022,V,5556.8002,N,03737.2609,E,9999.99,999.99,090426,,*26\\r\\n$GPVTG,999.99,T,,M,\",\"rsp\" : \"ok\"}";
	//Str = "{\n\"ep\": \"\uart_1\",\"cmd\" : \"receive\",\"data\" : \"""Vж\u00199GPRMC,184649.022,V,5556.8002,N,03737.2609,E,9999.99,999.99,090426,,*26\\r\\n$GPVTG,999.99,T,,M,\",\"rsp\" : \"ok\"}\r\n";
	//SStr << Str;

	std::string StrNew;
	for (auto i : Str)
	{
		if (i < 0x20 && i != '\r' && i != '\n')
		{
			StrNew += '.';
		}
		else
		{
			StrNew += i;
		}
	}

	SStr << StrNew;
	//SStr << TransactionJSON(MakeCmdJSON(ep, "receive"));
	boost::property_tree::ptree PTree;
	try
	{
		boost::property_tree::json_parser::read_json(SStr, PTree);
		std::string Data = PTree.get<std::string>("data");
		return std::vector<std::uint8_t>(Data.begin(), Data.end());
	}
	catch (std::exception e)
	{
		std::string What = e.what();
		int sd = 9;
	}
	
	return {};
}

void tTWRClient::TransactionJSON_UART_Send(tTWREndpoint ep, const std::string& tx)
{
	TransactionJSON(MakeCmdJSON(ep, "send", tx));
	//std::stringstream SStr;
	//SStr << TransactionJSON(MakeCmdJSON(ep, "open"));
	//boost::property_tree::ptree PTree;
	//boost::property_tree::json_parser::read_json(SStr, PTree);
	//std::string Ans = PTree.get<std::string>("rsp");
	//return Ans == "ok";
}

void tTWRClient::TransactionJSON_UART_Send(tTWREndpoint ep, const std::vector<std::uint8_t>& tx)
{
	TransactionJSON(MakeCmdJSON(ep, "send", std::string(tx.begin(), tx.end())));
}

tTWRPacketRsp tTWRClient::Transaction(const tTWRPacketCmd& cmd)
{
	std::vector<std::uint8_t> ReceivedData = Transaction<std::vector<std::uint8_t>, 1024>(cmd.ToVector());
	std::optional<tTWRPacketRsp> RspOpt = tTWRPacketRsp::Find(ReceivedData);
	if (!RspOpt.has_value())
		return {};
	return *RspOpt;
}

std::string tTWRClient::TransactionJSON(const std::string& cmdJSON)
{
	return Transaction<std::string, 4096>(cmdJSON);
}

std::string tTWRClient::MakeCmdJSON(tTWREndpoint ep, const std::string& cmd, const std::string& data)
{
	std::string Endpoint;
	switch (ep)
	{
	case tTWREndpoint::Control: Endpoint = "control"; break;
	case tTWREndpoint::UART0: Endpoint = "uart_0"; break;
	case tTWREndpoint::UART1: Endpoint = "uart_1"; break;
	case tTWREndpoint::UART2: Endpoint = "uart_2"; break;
	case tTWREndpoint::UART3: Endpoint = "uart_3"; break;
	case tTWREndpoint::UART4: Endpoint = "uart_4"; break;
	case tTWREndpoint::UART5: Endpoint = "uart_5"; break;
	default: return {};
	}
	std::string Str = "{\"ep\":\"" + Endpoint + "\",\"cmd\":\"" + cmd + "\"";
	if (!data.empty())
		Str += ",\"data\":\"" + data + "\\r\\n\"";
	Str += "}";
	return Str;
}

std::string tTWRClient::MakeCmdJSON(tTWREndpoint ep, const std::string& cmd)
{
	return MakeCmdJSON(ep, cmd, {});
}

void tTWRClient::SetState(tState state)
{
	m_State = state;
	m_cv.notify_all();
}

void tTWRClient::CtrlState()
{
	while (m_State != tState::Close)
	{
		switch (m_State)
		{
		case  tState::None:
		{
			std::unique_lock<std::mutex> Lock(m_cv_mtx);
			m_cv.wait(Lock);
			break;
		}
		case tState::Read:
		case tState::Write:
		{
			std::unique_lock<std::mutex> Lock(m_cv_mtx);
			if (m_cv.wait_for(Lock, std::chrono::seconds(1)) == std::cv_status::timeout) // [#] < 1 s. That is available period of time to send/receive.
			{
				// It breaks the receiving process through blocking socket (receive_from) if no data can be received for any reason.
				boost::system::error_code ec; // It's here in order to avoid an exception.
				if (m_Socket.is_open())
					m_Socket.shutdown(boost::asio::socket_base::shutdown_both, ec);
				m_Socket.close(ec);

				SetState(tState::Close);
			}
			break;
		}
		}
	}
}

void tTWRClient::CtrlStateThread(void* obj)
{
	auto TWRClient = static_cast<tTWRClient*>(obj);
	TWRClient->CtrlState();
}

}
}
#endif // MXTWR_CLIENT
