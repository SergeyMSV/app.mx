#include "devDataSetConfig.h"
#include "main.h"
#include "main_server.h"

#include <sharePortGPIO.h>
#include <sharePortSPI.h>

#include <memory>

void Thread_Port_SPI(const std::shared_ptr<dev::tDataSetConfig>& dataSetConfig, tTWRServer& twrServer, tTWRQueueSPICmd& twrQueue)
{
	std::weak_ptr<dev::tDataSetConfig> DataSetConfig_WeakPtr(dataSetConfig);

	const share_config::tSPIPort ConfSPI = dataSetConfig->GetSPI0_CS0();
	if (ConfSPI.IsWrong())
		return;
	const share_config::tGPIOPort ConfRST = dataSetConfig->GetSPI0_CS0_RST();
	if (ConfRST.IsWrong())
		return;

	share_port::tSPI SPI(ConfSPI.ID, ConfSPI.Mode, ConfSPI.Bits, ConfSPI.Frequency_hz, ConfSPI.Delay_us);
	share_port::tGPIO RST(ConfRST.ID);

	int PeriodCounter = 0;

	while (true)
	{
		if (DataSetConfig_WeakPtr.expired())
			return;

		if (!twrQueue.empty())
		{
			PeriodCounter = 0;

			boost::shared_ptr<tVectorUInt8> PacketRsp;

			tPacketTWRCmdEp Cmd = twrQueue.get_front();
			switch (Cmd.Value.GetMsgId())
			{
			case TWR::tMsgId::SPI_Request:
			{
				if (!SPI.IsReady())
				{
					PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd.Value, TWR::tMsgStatus::Message, SPI.GetErrMsg()).ToVector()));
					break;
				}
				tVectorUInt8 Data = SPI.Transaction(Cmd.Value.GetPayload());
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make(Cmd.Value, Data).ToVector()));
				break;
			}
			case TWR::tMsgId::SPI_GetSettings:
			{
				std::uint8_t Mode = SPI.GetMode();
				std::uint8_t Bits = SPI.GetBits();
				std::uint32_t Speed = SPI.GetSpeed();
				std::uint16_t Delay = SPI.GetDelay();
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make(Cmd.Value, TWR::tSPIPortSettings{ Mode, Bits, Speed, Delay }).ToVector()));
				break;
			}
			case TWR::tMsgId::SPI_SetChipControl:
			{
				if (!RST.IsReady())
				{
					PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd.Value, TWR::tMsgStatus::Message, RST.GetErrMsg()).ToVector()));
					break;
				}
				tVectorUInt8 Data = Cmd.Value.GetPayload();
				if (Data.size() != sizeof(TWR::tChipControl))
				{
					PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd.Value, TWR::tMsgStatus::WrongPayloadSize).ToVector()));
					break;
				}
				TWR::tChipControl ChipCtrl;
				ChipCtrl.Value = Data[0];
				RST.SetState(ChipCtrl.Field.Reset);
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make(Cmd.Value).ToVector()));
				break;
			}
			}

			if (PacketRsp.get() == nullptr)
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd.Value, TWR::tMsgStatus::NotSupported).ToVector()));

			twrServer.Send(Cmd.Endpoint, PacketRsp);
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
