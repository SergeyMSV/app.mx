#include "devDataSetConfig.h"
#include "main.h"
#include "main_server.h"

#include <sharePortGPIO.h>
#include <sharePortSPI.h>

#include <memory>

void ThreadPortSPI(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server, tTWRQueueSPICmd& queueIn)
{
	std::weak_ptr<dev::tDataSetConfig> ConfigWeak(config);

	share::config::port::tSPI_Config ConfSPI;
	share::config::port::tGPIO_Config ConfRST;
	{
		std::shared_ptr<dev::tDataSetConfig> Conf = ConfigWeak.lock();
		ConfSPI = Conf->GetSPI0_CS0();
		if (ConfSPI.IsWrong())
			return; // [TBD] throw an exception
		ConfRST = Conf->GetSPI0_CS0_RST();
		if (ConfRST.IsWrong())
			return; // [TBD] throw an exception
	}

	share::port::tSPI SPI(ConfSPI.ID, ConfSPI.Mode, ConfSPI.Bits, ConfSPI.Frequency_hz, ConfSPI.Delay_us);
	share::port::tGPIO RST(ConfRST.ID);

	int PeriodCounter = 0;

	while (!ConfigWeak.expired())
	{
		if (queueIn.empty())
		{
			if (PeriodCounter < 100) // if SPI is not requested within 100 ms, the thread frequency must be reduced
			{
				++PeriodCounter;
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100 ms
			}
		}

		PeriodCounter = 0;

		boost::shared_ptr<std::vector<std::uint8_t>> PacketRsp;

		tPacketTWRCmdEp Cmd = queueIn.get_front();
		switch (Cmd.Value.GetMsgId())
		{
		case tTWRMsgId::SPI_Request:
		{
			if (!SPI.IsReady())
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::Message, SPI.GetErrMsg()).ToVector()));
				break;
			}
			std::vector<std::uint8_t> Data = SPI.Transaction(Cmd.Value.GetPayload());
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value, Data).ToVector()));
			break;
		}
		case tTWRMsgId::SPI_GetSettings:
		{
			std::uint8_t Mode = SPI.GetMode();
			std::uint8_t Bits = SPI.GetBits();
			std::uint32_t Speed = SPI.GetSpeed();
			std::uint16_t Delay = SPI.GetDelay();
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value, tTWRSPIPortSettings{ Mode, Bits, Speed, Delay }).ToVector()));
			break;
		}
		case tTWRMsgId::SPI_SetChipControl:
		{
			if (!RST.IsReady())
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::Message, RST.GetErrMsg()).ToVector()));
				break;
			}
			std::vector<std::uint8_t> Data = Cmd.Value.GetPayload();
			if (Data.size() != sizeof(tTWRChipControl))
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::WrongPayloadSize).ToVector()));
				break;
			}
			tTWRChipControl ChipCtrl;
			ChipCtrl.Value = Data[0];
			RST.SetState(ChipCtrl.Field.Reset);
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value).ToVector()));
			break;
		}
		}

		if (PacketRsp.get() == nullptr)
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::NotSupported).ToVector()));

		server.Send(Cmd.Endpoint, PacketRsp);
	}
}
