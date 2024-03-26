#include "main.h"

#include <shareNetwork.h>

#include <iostream>
#include <string>

namespace asio_ip = boost::asio::ip;

#ifdef UDP_SERVER_TEST

void UDP_ClientTest(std::uint16_t port)
{
	constexpr std::string_view Host{ "127.0.0.1" };

	int Counter = 0;

	while (true)
	{
		try
		{
			boost::asio::io_context ioc;

			asio_ip::udp::resolver Resolver(ioc);
			asio_ip::udp::endpoint ReceiverEndpoint = *Resolver.resolve(asio_ip::udp::v4(), Host, std::to_string(port)).begin();

			asio_ip::udp::socket Socket(ioc);
			Socket.open(asio_ip::udp::v4());

			std::string Pack;
			switch (Counter++)
			{
			case 0:
			{
				Pack = "{\"cmd\":\"search\"}";
				break;
			}
			case 1:
			{
				Pack = "{\"cmd\":\"search\",\"family_code\":\"28\"}"; // maybe it's better to replace with real family code like DS18B20 = 0x28, // Thermometer
				break;
			}
			case 2:
			{
				Pack = "{\"cmd\":\"search\",\"family_code\":\"01\"}"; // maybe it's better to replace with real family code like DS1990A = 0x01, // iButton
				break;
			}
			case 3:
			{
				Pack = "{\"cmd\":\"read_rom\"}";
				break;
			}
			case 4:
			{
				Pack = "{\"cmd\":\"thermo\",\"ids\":[\"c63e770e64ff\"]}"; // maybe it's better to replace with real family code like DS1990A = 0x01, // iButton
				break;
			}
			case 5:
			{
				Pack = "{\"cmd\":\"thermo\",\"ids\":[\"c63e770e64ff\",\"8747770e64ff\"]}"; // maybe it's better to replace with real family code like DS1990A = 0x01, // iButton
				break;
			}
			case 6:
			{
				Pack = "{\"cmd\":\"strange\"}";
				break;
			}
			case 7:
			{
				Pack = "{\"cmd\":\"version\"}";
				break;
			}
			case 8:
			{
				Pack = "{\"cmd\":\"read_key\"}";
				break;
			}
			case 9:
			{
				Pack = "{\"cmd\":\"write_key\",\"id\":\"00000308c4de\"}";
				break;
			}
			//case 10:
			//{
			//	Pack = "{\"cmd\":\"close\"}";
			//	break;
			//}
			default:
				Counter = 0;
				continue;
			}
			Socket.send_to(boost::asio::buffer(Pack.data(), Pack.size()), ReceiverEndpoint);

			///

			std::array<char, share_network_udp::PacketSizeMax> ReceiveBuffer;
			asio_ip::udp::endpoint SenderEndpoint;
			std::size_t Size = Socket.receive_from(boost::asio::buffer(ReceiveBuffer), SenderEndpoint);
			if (Size)
			{
				std::string ReceivedStr(ReceiveBuffer.begin(), ReceiveBuffer.begin() + Size);
				std::cout << "\nRSP\n" << ReceivedStr << '\n';
			}
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

#endif // UDP_SERVER_TEST
