///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketTWR
// 2017-12-01
// C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsCRC.h"
#include "utilsPacketStar.h"

namespace utils
{
namespace packet
{
namespace twr
{

enum class tMsgId : std::uint8_t
{
	None,
	Restart,
	GetVersion,
	DEMO_Request,
	GPIO_GetState = 0x10,
	GPIO_SetState,
	//GPIO_ChipEnable, // It depends on the Endpoint
	//GPIO_ChipReset, // It depends on the Endpoint
	//GPIO_GetIRQ1 = 0x10,
	//GPIO_GetIRQ2,
	//GPIO_SetEnable = 0x20,
	//GPIO_SetReset,
	//GPIO_SetWriteProtect,
	//I2C_Start = 0x30,
	//I2C_StartRepeated,
	//I2C_Read,
	//I2C_Write,
	//I2C_Stop,
	SPI_Open = 0x40,
	SPI_Close,
	SPI_Request,
	SPI_GetSettings,
	SPI_SetSettings,
	SPI_SetChipControl, // Enable, RESET, etc.
	UART_Receive = 0x50,
	UART_Send,
	UART_GetSettings,
	UART_SetSettings,
	UART_SetRTS,
	UART_GetCTS,
	UART_SetDTR,
	UART_SetDSR,
};

enum class tMsgStatus : std::uint8_t
{
	None = 0x00, // Success
	WrongEndpointMsg = 0x01, // Endpoint doesn't support received message
	WrongPayloadSize = 0x02,
	WrongPayloadFormat = 0x03,

	NotAvailable = 0xFC,
	NotSupported = 0xFD,
	Message = 0xFE, // Response packet contains an error message instead of its datagram.
	Unknown = 0xFF,
};

enum class tEndpoint : std::uint8_t
{
	Control,
	DEMO,
	CAN0 = 0x10,
	CAN1 = 0x20,
	I2C0 = 0x30,
	I2C1 = 0x40,
	ISO7816 = 0x50,
	RS485 = 0x60,
	SPI0_CS0 = 0x70, // from 0x70 up to 0x9F;
	SPI0_CS1,
	SPI0_CS2,
	SPI1_CS0,
	SPI1_CS1,
	SPI1_CS2,
	SPI2_CS0,
	SPI2_CS1,
	SPI2_CS2,
	SPI_END, // It can be equal to 0xA0, because it definitely is not a part of SPI.
	UART0 = 0xA0, // from 0xA0 up to 0xAF; 16 ports
	UART1,
	UART2,
	UART3,
	UART4,
	UART5,
	UART_END, // It can be equal to 0xB0, because it definitely is not a part of UART.
	// 0xB0 - free
	// 0xC0 - free
	GPIO_00 = 0xD0, // from 0xD0 up to 0xFF; D,E,F => 16 * 3 = 48 pins.
	GPIO_01,
	GPIO_02,
	GPIO_03,
	GPIO_04,
	GPIO_05,
	GPIO_06,
	GPIO_07,
	GPIO_08,
	GPIO_09,
};

#pragma pack(push, 1)
union tChipControl
{
	struct 
	{
		std::uint8_t Enable : 1;
		std::uint8_t Reset : 1;
		std::uint8_t : 6;
	}Field;
	std::uint8_t Value = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct tSPIPortSettings
{
	std::uint8_t Mode = 0;
	std::uint8_t Bits = 0;
	std::uint32_t Frequency_hz = 0;
	std::uint16_t Delay_us = 0;
};
#pragma pack(pop)

class tPayloadData // [MsgId 1-Byte][MsgStatus (for Rsp; in case of Cmd = 0) 1-Byte][Endpoint 1-Byte][Payload, its size can be up to 1022-Bytes]
{
	static constexpr std::size_t m_HeaderSize = 3;

public:
	tMsgId MsgId = tMsgId::None;
	tMsgStatus MsgStatus = tMsgStatus::None; // it's used for Rsp-packet
	tEndpoint Endpoint = tEndpoint::Control;
	std::vector<std::uint8_t> Payload; // Payload for Cmd and Rsp must be of the same size. It relates to SPI request for reading.

	tPayloadData() = default;
	tPayloadData(tMsgId id, tEndpoint ep, const std::vector<std::uint8_t>& payload)
		:MsgId(id), Endpoint(ep), Payload(payload)
	{}

	tPayloadData(std::vector<std::uint8_t>::const_iterator cbegin, std::vector<std::uint8_t>::const_iterator cend)
	{
		const std::size_t DataSize = std::distance(cbegin, cend);
		if (DataSize < m_HeaderSize)
			return;

		MsgId = static_cast<tMsgId>(*cbegin);
		MsgStatus = static_cast<tMsgStatus>(*(cbegin + 1));
		Endpoint = static_cast<tEndpoint>(*(cbegin + 2));
		Payload = std::vector<std::uint8_t>(cbegin + m_HeaderSize, cend);
	}

	bool empty() const { return size() == 0; }

	std::size_t size() const
	{
		return m_HeaderSize + Payload.size();
	}

	std::uint8_t operator[] (const std::size_t index) const
	{
		if (index >= size())
			return 0;

		switch (index)
		{
		case 0: return static_cast<std::uint8_t>(MsgId);
		case 1: return static_cast<std::uint8_t>(MsgStatus);
		case 2: return static_cast<std::uint8_t>(Endpoint);
		}
		return Payload[index - m_HeaderSize];
	}

	bool operator == (const tPayloadData& val) const = default;
	bool operator != (const tPayloadData& val) const = default;
};

struct tPayloadBase : public utils::packet::tPayload<tPayloadData>
{
	tPayloadBase() = default;
	tPayloadBase(std::vector<std::uint8_t>::const_iterator cbegin, std::vector<std::uint8_t>::const_iterator cend)
		:tPayload(cbegin, cend)
	{}
};

using tPacketBaseBase = utils::packet::tPacket<utils::packet::star::tFormat, tPayloadBase>;

class tPacketBase : public tPacketBaseBase
{
public:
	tPacketBase() = default;
	explicit tPacketBase(const payload_value_type& payloadValue) = delete;
	explicit tPacketBase(payload_value_type&& payloadValue)
		: tPacket(std::move(payloadValue))
	{
	}

	tMsgId GetMsgId() const { return GetPayloadValue().MsgId; }
	tMsgStatus GetMsgStatus() const { return GetPayloadValue().MsgStatus; }
	tEndpoint GetEndpoint() const { return  GetPayloadValue().Endpoint; }
	std::vector<std::uint8_t> GetPayload() const { return GetPayloadValue().Payload; }

	static bool CheckEndpointSPI(tEndpoint ep) { return ep >= tEndpoint::SPI0_CS0 && ep < tEndpoint::SPI_END; }
};

class tPacketCmd : public tPacketBase
{
	explicit tPacketCmd(const payload_value_type& payloadValue) = delete;
	explicit tPacketCmd(payload_value_type&& payloadValue)
		:tPacketBase(std::move(payloadValue))
	{
	}

public:
	tPacketCmd() = default;
	explicit tPacketCmd(const tPayloadBase& payload) = delete;
	explicit tPacketCmd(tPayloadBase&& payload)
	{
		*static_cast<tPayloadBase*>(this) = std::move(payload);
	}

	static std::optional<tPacketCmd> Find(std::vector<std::uint8_t>& data)
	{
		std::optional<tPacketBaseBase> PacketOpt = tPacketBaseBase::Find(data);
		if (!PacketOpt.has_value())
			return {};
		return tPacketCmd(std::move(*PacketOpt));
	}

	static tPacketCmd Make_Restart()
	{
		return Make(tMsgId::Restart, tEndpoint::Control, {});
	}

	static tPacketCmd Make_GetVersion()
	{
		return Make(tMsgId::GetVersion, tEndpoint::Control, {});
	}

	static tPacketCmd Make_DEMO_Request(tEndpoint ep, const std::vector<std::uint8_t>& msgData)
	{
		assert(ep == tEndpoint::DEMO);
		return Make(tMsgId::DEMO_Request, ep, msgData);
	}

	static tPacketCmd Make_GPIO_GetState(tEndpoint ep)
	{
		// [TBD] verify endpoint from args, if it is wrong an exception must be thrown
		return Make(tMsgId::GPIO_GetState, ep, {});
	}

	static tPacketCmd Make_GPIO_SetState(tEndpoint ep, bool state)
	{
		// [TBD] verify endpoint from args, if it is wrong an exception must be thrown
		std::vector<std::uint8_t> Data;
		Data.push_back(state ? 0x01 : 0x00);
		return Make(tMsgId::GPIO_SetState, ep, Data);
	}

	static tPacketCmd Make_SPI_Open(tEndpoint ep)
	{
		assert(CheckEndpointSPI(ep));
		return Make(tMsgId::SPI_Open, ep, {});
	}

	static tPacketCmd Make_SPI_Close(tEndpoint ep)
	{
		assert(CheckEndpointSPI(ep));
		return Make(tMsgId::SPI_Close, ep, {});
	}

	static tPacketCmd Make_SPI_Request(tEndpoint ep, const std::vector<std::uint8_t>& msgData)
	{
		assert(CheckEndpointSPI(ep));
		return Make(tMsgId::SPI_Request, ep, msgData);
	}

	static tPacketCmd Make_SPI_GetSettings(tEndpoint ep)
	{
		assert(CheckEndpointSPI(ep));
		return Make(tMsgId::SPI_GetSettings, ep, {});
	}

	static tPacketCmd Make_SPI_SetSettings(tEndpoint ep, uint32_t freq)
	{
		assert(CheckEndpointSPI(ep));
		std::vector<std::uint8_t> Data = utils::ToVector(freq);
		return Make(tMsgId::SPI_SetSettings, ep, Data);
	}

	static tPacketCmd Make_SPI_SetChipControl(tEndpoint ep, tChipControl ctrl)
	{
		assert(CheckEndpointSPI(ep));
		std::vector<std::uint8_t> Data;
		Data.push_back(ctrl.Value);
		return Make(tMsgId::SPI_SetChipControl, ep, Data);
	}

	// ... Make-functions for other packets

private:
	static tPacketCmd Make(tMsgId id, tEndpoint ep, const std::vector<std::uint8_t>& msgData)
	{
		payload_value_type Pld;
		Pld.MsgId = id;
		Pld.Endpoint = ep;
		Pld.Payload = msgData;
		return tPacketCmd(std::move(Pld));
	}
};

class tPacketRsp : public tPacketBase
{
	explicit tPacketRsp(const payload_value_type& payloadValue) = delete;
	explicit tPacketRsp(payload_value_type&& payloadValue)
		:tPacketBase(std::move(payloadValue))
	{
	}

public:
	tPacketRsp() = default;
	explicit tPacketRsp(const tPayloadBase& payload) = delete;
	explicit tPacketRsp(tPayloadBase&& payload)
	{
		*static_cast<tPayloadBase*>(this) = std::move(payload);
	}

	static std::optional<tPacketRsp> Find(std::vector<std::uint8_t>& data)
	{
		std::optional<tPacketBaseBase> PacketOpt = tPacketBaseBase::Find(data);
		if (!PacketOpt.has_value())
			return {};
		return tPacketRsp(std::move(*PacketOpt));
	}

	static tPacketRsp Make(const tPacketBase& pack)
	{
		return Make(pack.GetMsgId(), tMsgStatus::None, pack.GetEndpoint(), {});
	}

	template <class T>
	static tPacketRsp Make(const tPacketBase& pack, const T& data)
	{
		return Make(pack.GetMsgId(), tMsgStatus::None, pack.GetEndpoint(), utils::ToVector(data));
	}

	static tPacketRsp Make(const tPacketBase& pack, const std::vector<std::uint8_t>& data)
	{
		return Make(pack.GetMsgId(), tMsgStatus::None, pack.GetEndpoint(), data);
	}

	static tPacketRsp Make_ERR(const tPacketBase& pack, tMsgStatus status)
	{
		return Make(pack.GetMsgId(), status, pack.GetEndpoint(), {});
	}

	static tPacketRsp Make_ERR(const tPacketBase& pack, tMsgStatus status, const std::string& data)
	{
		return Make(pack.GetMsgId(), status, pack.GetEndpoint(), { data.begin(), data.end() });
	}

private:
	static tPacketRsp Make(tMsgId id, tMsgStatus status, tEndpoint ep, const std::vector<std::uint8_t>& msgData)
	{
		payload_value_type Pld;
		Pld.MsgId = id;
		Pld.MsgStatus = status;
		Pld.Endpoint = ep;
		Pld.Payload = msgData;
		return tPacketRsp(std::move(Pld));
	}
};

}
}
}
