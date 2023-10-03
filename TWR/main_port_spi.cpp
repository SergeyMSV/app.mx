#include "devDataSetConfig.h"
#include "main.h"
#include "main_server.h"

#include <sharePortGPIO.h>
#include <sharePortSPI.h>

void Thread_Port_SPI(const share_config::tSPIPort& confSPI, const share_config::tGPIOPort& confRST, tTWRServer& twrServer, tTWRQueueSPICmd& twrQueue)
{
	share_port::tSPI SPI(confSPI.ID, confSPI.Mode, confSPI.Bits, confSPI.Frequency_hz, confSPI.Delay_us);
	share_port::tGPIO RST(confRST.ID);

	int PeriodCounter = 0;

	while (true)
	{
		if (!twrQueue.empty())
		{
			PeriodCounter = 0;

			boost::shared_ptr<tVectorUInt8> PacketRsp;

			tPacketTWRCmd Cmd = twrQueue.front();
			switch (Cmd.GetMsgId())
			{
			case TWR::tMsgId::SPI_Request:
			{
				if (!SPI.IsReady())
				{
					PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd, TWR::tMsgStatus::Message, SPI.GetErrMsg()).ToVector()));
					break;
				}
				tVectorUInt8 Data = SPI.Transaction(Cmd.GetPayload());
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make(Cmd, Data).ToVector()));
				break;
			}
			case TWR::tMsgId::SPI_GetSettings:
			{
				std::uint8_t Mode = SPI.GetMode();
				std::uint8_t Bits = SPI.GetBits();
				std::uint32_t Speed = SPI.GetSpeed();
				std::uint16_t Delay = SPI.GetDelay();
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make(Cmd, TWR::tSPIPortSettings{ Mode, Bits, Speed, Delay }).ToVector()));
				break;
			}
			case TWR::tMsgId::SPI_SetChipControl:
			{
				if (!RST.IsReady())
				{
					PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd, TWR::tMsgStatus::Message, RST.GetErrMsg()).ToVector()));
					break;
				}
				tVectorUInt8 Data = Cmd.GetPayload();
				if (Data.size() != sizeof(TWR::tChipControl))
				{
					PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd, TWR::tMsgStatus::WrongPayloadSize).ToVector()));
					break;
				}
				TWR::tChipControl ChipCtrl;
				ChipCtrl.Value = Data[0];
				RST.SetState(ChipCtrl.Field.Reset);
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make(Cmd).ToVector()));
				break;
			}
			}

			if (PacketRsp.get() == nullptr)
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd, TWR::tMsgStatus::NotSupported).ToVector()));

			twrServer.Send(PacketRsp);
		}
		else if (PeriodCounter < 100) // if SPI is not requested within 100 ms, the thread frequency must be reduced
		{
			++PeriodCounter;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100 ms
		}
	}
}
