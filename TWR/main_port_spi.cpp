#include "devDataSetConfig.h"
#include "main.h"
#include "main_server.h"

#include <sharePortGPIO.h>
#include <sharePortSPI.h>

#include <memory>

class tPortSPIHolder
{
	share::port::tSPI m_SPI;
	share::port::tGPIO m_RST;

public:
	tPortSPIHolder() = delete;
	tPortSPIHolder(const share::config::port::tSPI_Config& configSPI, const share::config::port::tGPIO_Config& configRST)
		:m_SPI(configSPI.ID, configSPI.Mode, configSPI.Bits, configSPI.Frequency_hz, configSPI.Delay_us), m_RST(configRST.ID)
	{
	}

	bool IsReady() const { return m_SPI.IsReady(); }
	std::string GetErrMsg() const { return m_SPI.GetErrMsg(); }

	std::uint8_t GetMode() { return m_SPI.GetMode(); }
	bool SetMode(std::uint8_t val) { return m_SPI.SetMode(val); }
	std::uint8_t GetBits() { return m_SPI.GetBits(); }
	bool SetBits(std::uint8_t val) { return m_SPI.SetBits(val); }
	std::uint32_t GetSpeed() { return m_SPI.GetSpeed(); }
	bool SetSpeed(std::uint32_t val) { return m_SPI.SetSpeed(val); }
	std::uint16_t GetDelay() const { return m_SPI.GetDelay(); }
	void SetDelay(std::uint16_t val) { m_SPI.SetDelay(val); }

	std::vector<std::uint8_t> Transaction(const std::vector<std::uint8_t>& tx) { return m_SPI.Transaction(tx); }

	void SetRST(bool state) { m_RST.SetState(state); }
};

static void ThreadPortSPI(const std::shared_ptr<dev::tDataSetConfig>& config, const share::config::port::tSPI_Config& configSPI, share::config::port::tGPIO_Config configRST, tTWRServer& server, tTWRQueueSPICmd& queueIn)
{
	std::weak_ptr<dev::tDataSetConfig> ConfigWeak(config);

	using tPortHld = tPortSPIHolder;

	std::unique_ptr<tPortHld> PortPtr;

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
			continue;
		}

		PeriodCounter = 0;

		boost::shared_ptr<std::vector<std::uint8_t>> PacketRsp;

		const tPacketTWRCmdEp Cmd = queueIn.get_front();
		switch (Cmd.Value.GetMsgId())
		{
		case tTWRMsgId::SPI_Open:
		{
			if (PortPtr)
				PortPtr.reset();
			PortPtr = std::make_unique<tPortHld>(configSPI, configRST);
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value).ToVector()));
			break;
		}
		case tTWRMsgId::SPI_Close:
		{
			PortPtr.reset();
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value).ToVector()));
			break;
		}
		case tTWRMsgId::SPI_Request:
		{
			if (!PortPtr)
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::NotAvailable).ToVector()));
				break;
			}

			if (!PortPtr->IsReady())
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::Message, PortPtr->GetErrMsg()).ToVector()));
				break;
			}

			std::vector<std::uint8_t> Data = PortPtr->Transaction(Cmd.Value.GetPayload());
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value, Data).ToVector()));
			break;
		}
		case tTWRMsgId::SPI_GetSettings:
		{
			if (!PortPtr)
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::NotAvailable).ToVector()));
				break;
			}

			if (!PortPtr->IsReady())
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::Message, PortPtr->GetErrMsg()).ToVector()));
				break;
			}

			std::uint8_t Mode = PortPtr->GetMode();
			std::uint8_t Bits = PortPtr->GetBits();
			std::uint32_t Speed = PortPtr->GetSpeed();
			std::uint16_t Delay = PortPtr->GetDelay();
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value, tTWRSPIPortSettings{ Mode, Bits, Speed, Delay }).ToVector()));
			break;
		}
		case tTWRMsgId::SPI_SetChipControl:
		{
			if (!PortPtr)
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::NotAvailable).ToVector()));
				break;
			}

			if (!PortPtr->IsReady())
			{
				PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::Message, PortPtr->GetErrMsg()).ToVector()));
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
			PortPtr->SetRST(ChipCtrl.Field.Reset);
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value).ToVector()));
			break;
		}
		}

		if (PacketRsp.get() == nullptr)
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::NotSupported).ToVector()));

		server.Send(Cmd.Endpoint, PacketRsp);
	}
}

void ThreadPortSPI0_CS0(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server)
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

	ThreadPortSPI(config, ConfSPI, ConfRST, server, TWRQueue.SPI0_CS0);
}
