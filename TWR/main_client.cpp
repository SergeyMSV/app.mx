#include "main.h"

#include <iostream>
#include <string>

namespace asio_ip = boost::asio::ip;

#ifdef UDP_SERVER_TEST

void UDP_ClientTest(std::uint16_t port)
{
	constexpr std::string_view Host{ "127.0.0.1" };
	//constexpr std::string_view Host{ "192.168.10.161" };
	//constexpr std::string_view Host{ "192.168.10.162" };

	auto ShowPackRsp = [](const tTWRPacketBase& pack)
	{
		std::cout << "Rsp MsgId = " << (int)pack.GetMsgId() << "; MsgStatus = " << (int)pack.GetMsgStatus() << "; Ep = " << (int)pack.GetEndpoint() << ";\n";
		std::cout << "<- ";

		if (pack.GetMsgId() == tTWRMsgId::GetVersion ||
			pack.GetMsgId() == tTWRMsgId::DEMO_Request ||
			pack.GetMsgStatus() == tTWRMsgStatus::Message)
		{
			std::vector<std::uint8_t> MsgVec = pack.GetPayload();
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
			boost::asio::io_context ioc;

			asio_ip::udp::resolver Resolver(ioc);
			asio_ip::udp::endpoint ReceiverEndpoint = *Resolver.resolve(asio_ip::udp::v4(), Host, std::to_string(port)).begin();

			asio_ip::udp::socket Socket(ioc);
			Socket.open(asio_ip::udp::v4());

			while (true)
			{
				std::vector<std::uint8_t> Pack;
				switch (Counter++)
				{
				case 0: Pack = tTWRPacketCmd::Make_Restart().ToVector(); break;
				case 1: Pack = tTWRPacketCmd::Make_GetVersion().ToVector(); break;
				case 2: Pack = tTWRPacketCmd::Make_DEMO_Request(tTWREndpoint::DEMO, { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38 }).ToVector(); break;
				case 3: Pack = tTWRPacketCmd::Make_SPI_Request(tTWREndpoint::SPI0_CS0, { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 }).ToVector(); break;
				case 4: Pack = tTWRPacketCmd::Make_SPI_GetSettings(tTWREndpoint::SPI0_CS0).ToVector(); break;
				case 5: Pack = tTWRPacketCmd::Make_SPI_GetSettings(tTWREndpoint::SPI2_CS0).ToVector(); break;
				case 6: Pack = tTWRPacketCmd::Make_SPI_SetChipControl(tTWREndpoint::SPI0_CS0, tTWRChipControl{}).ToVector(); break;
				case 7:
				{
					tTWRChipControl ChipControl;
					ChipControl.Field.Reset = 1;
					Pack = tTWRPacketCmd::Make_SPI_SetChipControl(tTWREndpoint::SPI0_CS0, ChipControl).ToVector();
					break;
				}
				default:
					Counter = 0;
					continue;
				}
				Socket.send_to(boost::asio::buffer(Pack.data(), Pack.size()), ReceiverEndpoint);

				///

				std::array<char, dev::settings::network_udp::PacketSizeMax> ReceiveBuffer;
				asio_ip::udp::endpoint SenderEndpoint;
				std::size_t Size = Socket.receive_from(boost::asio::buffer(ReceiveBuffer), SenderEndpoint);
				if (Size)
				{
					std::vector<std::uint8_t> ReceivedData(ReceiveBuffer.begin(), ReceiveBuffer.end());

					std::optional<tTWRPacketRsp> RspOpt = tTWRPacketRsp::Find(ReceivedData);
					if (RspOpt.has_value())
						ShowPackRsp(*RspOpt);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
