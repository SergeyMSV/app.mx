#include "main.h"

#include <iostream>
#include <string>
#include <sstream>

#include <utilsBase.h>

#include <shareNetwork.h>

#include <boost/property_tree/json_parser.hpp>

namespace asio_ip = boost::asio::ip;

#ifdef UDP_SERVER_TEST

#ifdef _WIN32
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
//https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersaddresses
// GetAdaptersAddresses использована для того, чтобы избавиться от проблем связанных с использованием getaddrinfo (см. ниже)
// используется внутри "ipconfig /all"
static std::vector<asio_ip::network_v4> GetHostAddresses()
{
	DWORD result = 0;
	ULONG addrBufSize = 15000; // [#]
	std::vector<std::uint8_t> addrBuf;
	for (int i = 0; i < 2; ++i) // it doesn't make sense to ack it more than twice
	{
		addrBuf.resize(addrBufSize);
		result = GetAdaptersAddresses(AF_INET,
			GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_INFO | GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME,
			NULL, (IP_ADAPTER_ADDRESSES*)addrBuf.data(), &addrBufSize);
		if (result != ERROR_BUFFER_OVERFLOW)
			break;
	}

	if (result != NO_ERROR)
		return {};

	std::vector<asio_ip::network_v4> Addrs;

	IP_ADAPTER_ADDRESSES* addresses = (IP_ADAPTER_ADDRESSES*)addrBuf.data();
	while (addresses)
	{
		IP_ADAPTER_UNICAST_ADDRESS* addrUnicast = addresses->FirstUnicastAddress;
		while (addrUnicast)
		{
			if (addrUnicast->Address.lpSockaddr->sa_family != AF_INET && addrUnicast->DadState != IpDadStatePreferred)
				continue;

			sockaddr_in* address = (sockaddr_in*)addrUnicast->Address.lpSockaddr;
			Addrs.push_back(asio_ip::make_network_v4(asio_ip::make_address_v4(utils::Reverse(address->sin_addr.s_addr)), static_cast<std::uint16_t>(addrUnicast->OnLinkPrefixLength)));

			addrUnicast = addrUnicast->Next;
		}
		addresses = addresses->Next;
	}

	return Addrs;
}
#endif // _WIN32

static bool Contains(const asio_ip::network_v4& network, const asio_ip::address_v4& ip)
{
	return network.canonical().address() == asio_ip::network_v4(ip, network.prefix_length()).canonical().address();
}

void UDP_ClientTest(std::uint16_t port)
{
	const asio_ip::network_v4 NetworkPreffered = asio_ip::make_network_v4(settings::Network);

	try
	{
		std::vector<asio_ip::network_v4> Networks;
#ifdef _WIN32
		Networks = GetHostAddresses();
#endif // _WIN32

		for (auto& i : Networks)
		{
			std::cout
				<< "-> " << i.address().to_string()
				<< " Mask: " << i.netmask().to_string()
				<< " Broad: " << i.broadcast().to_string()
				<< '\n';
		}

		asio_ip::network_v4 Network;
		for (auto& i : Networks)
		{
			if (i.address().is_loopback() || i.address().is_multicast() || i.address().is_unspecified())
				continue;
			if (Contains(NetworkPreffered, i.address()))
			{
				Network = i;
				break;
			}
		}

		if (Network == asio_ip::network_v4())
		{
			std::cerr << "The network hasn't been found.\n";
			return;
		}

		boost::asio::io_context ioc;
		asio_ip::udp::resolver Resolver(ioc);
		asio_ip::udp::socket Socket(ioc);

		asio_ip::udp::endpoint RecvEP(Network.broadcast(), port);

		Socket.open(asio_ip::udp::v4());

		std::thread ThreadReceive([&]()
			{
				while (true)
				{
					try
					{
						std::array<char, share_network_udp::PacketSizeMax> ReceiveBuffer;
						asio_ip::udp::endpoint SenderEndpoint;
						std::size_t Size = Socket.receive_from(boost::asio::buffer(ReceiveBuffer), SenderEndpoint);
						if (Size)
						{
							std::string Str(ReceiveBuffer.data(), Size - 1);
							std::istringstream SStr(std::move(Str));

							boost::property_tree::ptree PTree;
							boost::property_tree::json_parser::read_json(SStr, PTree);

							std::string PlatformID = PTree.get<std::string>("platform_id");
							std::string HostName = PTree.get<std::string>("hostname");

							std::cout
								<< " platform_id: " << PlatformID
								<< " hostname: " << HostName
								<< " ip: " << SenderEndpoint.address().to_string()
								<< '\n';
						}
					}
					catch (std::exception& e)
					{
						// [TBD] if connection is completely closed -> return
						std::cerr << e.what() << '\n';
					}
				}
			});

		int Counter = 0;
		while (true)
		{
			try
			{
				std::string Pack;
				switch (Counter++)
				{
				case 0: Pack = "get_tag"; break;
				default:
					Counter = 0;
					continue;
				}

				Socket.send_to(boost::asio::buffer(Pack.data(), Pack.size()), RecvEP);
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		ThreadReceive.join();
	}
	catch (...)
	{
	}
}

#endif // UDP_SERVER_TEST
