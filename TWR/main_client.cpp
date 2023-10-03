#include "main.h"

#include <iostream>
#include <string>

namespace asio_ip = boost::asio::ip;
namespace TWR = utils::packet_TWR;

using tPacketTWR = utils::packet_TWR::tPacketTWR;
using tPacketTWRCmd = utils::packet_TWR::tPacketTWRCmd;
using tPacketTWRRsp = utils::packet_TWR::tPacketTWRRsp;

#ifdef UDP_SERVER_TEST

void UDP_ClientTest(std::uint16_t port)
{
	constexpr std::string_view Host{ "127.0.0.1" };
	//constexpr std::string_view Host{ "192.168.10.161" };

	auto ShowPackRsp = [](const tPacketTWR& pack)
	{
		std::cout << "Rsp MsgId = " << (int)pack.GetMsgId() << "; MsgStatus = " << (int)pack.GetMsgStatus() << "; Ep = " << (int)pack.GetEndpoint() << ";\n";
		std::cout << "<- ";

		if (pack.GetMsgId() == TWR::tMsgId::GetVersion ||
			pack.GetMsgId() == TWR::tMsgId::DEMO_Request ||
			pack.GetMsgStatus() == TWR::tMsgStatus::Message)
		{
			tVectorUInt8 MsgVec = pack.GetPayload();
			std::string Msg(MsgVec.begin(), MsgVec.end());
			std::cout << Msg << '\n';
			return;
		}

		for (auto i : pack.GetPayload())
			std::cout << std::right << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(i) << ' ';
		std::cout << '\n';
	};

	int Counter = 0;

	while (true)
	{
		try
		{
			boost::asio::io_context IO_context;

			asio_ip::udp::resolver Resolver(IO_context);
			asio_ip::udp::endpoint ReceiverEndpoint = *Resolver.resolve(asio_ip::udp::v4(), Host, std::to_string(port)).begin();

			asio_ip::udp::socket Socket(IO_context);
			Socket.open(asio_ip::udp::v4());

			tVectorUInt8 Pack;
			switch (Counter++)
			{
			case 0: Pack = tPacketTWRCmd::Make_Restart().ToVector(); break;
			case 1: Pack = tPacketTWRCmd::Make_GetVersion().ToVector(); break;
			case 2: Pack = tPacketTWRCmd::Make_DEMO_Request(TWR::tEndpoint::DEMO, { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38 }).ToVector(); break;
			case 3: Pack = tPacketTWRCmd::Make_SPI_Request(TWR::tEndpoint::SPI0_CS0, { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 }).ToVector(); break;
			case 4: Pack = tPacketTWRCmd::Make_SPI_GetSettings(TWR::tEndpoint::SPI0_CS0).ToVector(); break;
			case 5: Pack = tPacketTWRCmd::Make_SPI_GetSettings(TWR::tEndpoint::SPI2_CS0).ToVector(); break;
			case 6: Pack = tPacketTWRCmd::Make_SPI_SetChipControl(TWR::tEndpoint::SPI0_CS0, TWR::tChipControl{}).ToVector(); break;
			case 7:
			{
				TWR::tChipControl ChipControl;
				ChipControl.Field.Reset = 1;
				Pack = tPacketTWRCmd::Make_SPI_SetChipControl(TWR::tEndpoint::SPI0_CS0, ChipControl).ToVector();
				break;
			}
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
				tVectorUInt8 ReceivedData(ReceiveBuffer.begin(), ReceiveBuffer.end());

				tPacketTWRRsp Rsp;
				std::size_t PackSize = tPacketTWRRsp::Find(ReceivedData, Rsp);
				if (PackSize)
					ShowPackRsp(Rsp);
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
