///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketTWR
// 2017-12-01
// Standard ISO/IEC 114882, C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsCRC.h"
#include "utilsPacketStar.h"

namespace utils
{
namespace packet_TWR
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
	SPI_Request = 0x40,
	SPI_GetSettings,
	SPI_SetSettings,
	SPI_SetChipControl, // Enable, RESET, etc.
};

enum class tMsgStatus : std::uint8_t
{
	None = 0x00, // Success
	WrongEndpointMsg = 0x01, // Endpoint doesn't support received message
	WrongPayloadSize = 0x02,
	WrongPayloadFormat = 0x03,

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
	SPI0_CS0 = 0x70,
	SPI0_CS1,
	SPI0_CS2,
	SPI1_CS0,
	SPI1_CS1,
	SPI1_CS2,
	SPI2_CS0,
	SPI2_CS1,
	SPI2_CS2,
	SPI_END,
	GPIO_00 = 0xA0,
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

class tPayloadTWRData // [MsgId 1-Byte][MsgStatus (for Rsp; in case of Cmd = 0) 1-Byte][Endpoint 1-Byte][Payload, its size can be up to 1022-Bytes]
{
	static constexpr std::size_t HeaderSize = 3;

public:
	tMsgId MsgId = tMsgId::None;
	tMsgStatus MsgStatus = tMsgStatus::None; // it's used for Rsp-packet
	tEndpoint Endpoint = tEndpoint::Control;
	tVectorUInt8 Payload; // Payload for Cmd and Rsp must be of the same size. It relates to SPI request for reading.

	tPayloadTWRData() = default;
	tPayloadTWRData(tMsgId id, tEndpoint ep, const tVectorUInt8& payload)
		:MsgId(id), Endpoint(ep), Payload(payload)
	{}

	tPayloadTWRData(tVectorUInt8::const_iterator cbegin, tVectorUInt8::const_iterator cend)
	{
		const std::size_t DataSize = std::distance(cbegin, cend);
		if (DataSize < HeaderSize)
			return;

		MsgId = static_cast<tMsgId>(*cbegin);
		MsgStatus = static_cast<tMsgStatus>(*(cbegin + 1));
		Endpoint = static_cast<tEndpoint>(*(cbegin + 2));
		Payload = tVectorUInt8(cbegin + HeaderSize, cend);
	}

	std::size_t size() const
	{
		return HeaderSize + Payload.size();
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
		return Payload[index - HeaderSize];
	}

	bool operator == (const tPayloadTWRData& val) const = default;
	bool operator != (const tPayloadTWRData& val) const = default;
};

struct tPayloadTWR : public packet::tPayload<tPayloadTWRData>
{
	tPayloadTWR() = default;
	tPayloadTWR(tVectorUInt8::const_iterator cbegin, tVectorUInt8::const_iterator cend)
		:tPayload(cbegin, cend)
	{}
};

class tPacketTWR : public packet::tPacket<packet_Star::tFormatStar, tPayloadTWR>
{
public:
	tPacketTWR() = default;
	explicit tPacketTWR(const payload_value_type& payloadValue)
		:tPacket(payloadValue)
	{}

	tMsgId GetMsgId() const { return GetPayloadValue().MsgId; }
	tMsgStatus GetMsgStatus() const { return GetPayloadValue().MsgStatus; }
	tEndpoint GetEndpoint() const { return  GetPayloadValue().Endpoint; }
	tVectorUInt8 GetPayload() const { return GetPayloadValue().Payload; }

	static bool CheckEndpointSPI(tEndpoint ep) { return ep >= tEndpoint::SPI0_CS0 && ep < tEndpoint::SPI_END; }
};

class tPacketTWRCmd : public tPacketTWR
{
	explicit tPacketTWRCmd(const payload_value_type& payloadValue)
		:tPacketTWR(payloadValue)
	{}

public:
	tPacketTWRCmd() = default;

	static tPacketTWRCmd Make_Restart()
	{
		return Make(tMsgId::Restart, tEndpoint::Control, {});
	}

	static tPacketTWRCmd Make_GetVersion()
	{
		return Make(tMsgId::GetVersion, tEndpoint::Control, {});
	}

	static tPacketTWRCmd Make_DEMO_Request(tEndpoint ep, const tVectorUInt8& msgData)
	{
		assert(ep == tEndpoint::DEMO);
		return Make(tMsgId::DEMO_Request, ep, msgData);
	}

	static tPacketTWRCmd Make_GPIO_GetState(tEndpoint ep)
	{
		// [TBD] verify endpoint from args, if it is wrong an exception must be thrown
		return Make(tMsgId::GPIO_GetState, ep, {});
	}

	static tPacketTWRCmd Make_GPIO_SetState(tEndpoint ep, bool state)
	{
		// [TBD] verify endpoint from args, if it is wrong an exception must be thrown
		tVectorUInt8 Data;
		Data.push_back(state ? 0x01 : 0x00);
		return Make(tMsgId::GPIO_SetState, ep, Data);
	}

	static tPacketTWRCmd Make_SPI_Request(tEndpoint ep, const tVectorUInt8& msgData)
	{
		assert(CheckEndpointSPI(ep));
		return Make(tMsgId::SPI_Request, ep, msgData);
	}

	static tPacketTWRCmd Make_SPI_GetSettings(tEndpoint ep)
	{
		assert(CheckEndpointSPI(ep));
		return Make(tMsgId::SPI_GetSettings, ep, {});
	}

	static tPacketTWRCmd Make_SPI_SetSettings(tEndpoint ep, uint32_t freq)
	{
		assert(CheckEndpointSPI(ep));
		tVectorUInt8 Data = utils::ToVector(freq);
		return Make(tMsgId::SPI_SetSettings, ep, Data);
	}

	static tPacketTWRCmd Make_SPI_SetChipControl(tEndpoint ep, tChipControl ctrl)
	{
		assert(CheckEndpointSPI(ep));
		tVectorUInt8 Data;
		Data.push_back(ctrl.Value);
		return Make(tMsgId::SPI_SetChipControl, ep, Data);
	}

	// ... Make-functions for other packets

private:
	static tPacketTWRCmd Make(tMsgId id, tEndpoint ep, const tVectorUInt8& msgData)
	{
		tPayloadTWRData Pld;
		//tPayloadTWR::value_type Pld; // GCC: "'struct utils::packet_TWR::tPayloadTWR utils::packet_TWR::tPayloadTWR::tPayloadTWR' is inaccessible within this context"
		Pld.MsgId = id;
		Pld.Endpoint = ep;
		Pld.Payload = msgData;
		return tPacketTWRCmd(Pld);
	}
};

class tPacketTWRRsp : public tPacketTWR
{
	explicit tPacketTWRRsp(const payload_value_type& payloadValue)
		:tPacketTWR(payloadValue)
	{}

public:
	tPacketTWRRsp() = default;

	static tPacketTWRRsp Make(const tPacketTWR& pack)
	{
		return Make(pack.GetMsgId(), tMsgStatus::None, pack.GetEndpoint(), {});
	}

	template <class T>
	static tPacketTWRRsp Make(const tPacketTWR& pack, const T& data)
	{
		return Make(pack.GetMsgId(), tMsgStatus::None, pack.GetEndpoint(), utils::ToVector(data));
	}

	static tPacketTWRRsp Make(const tPacketTWR& pack, const tVectorUInt8& data)
	{
		return Make(pack.GetMsgId(), tMsgStatus::None, pack.GetEndpoint(), data);
	}

	static tPacketTWRRsp Make_ERR(const tPacketTWR& pack, tMsgStatus status)
	{
		return Make(pack.GetMsgId(), status, pack.GetEndpoint(), {});
	}

	static tPacketTWRRsp Make_ERR(const tPacketTWR& pack, tMsgStatus status, const std::string& data)
	{
		return Make(pack.GetMsgId(), status, pack.GetEndpoint(), { data.begin(), data.end() });
	}

private:
	static tPacketTWRRsp Make(tMsgId id, tMsgStatus status, tEndpoint ep, const tVectorUInt8& msgData)
	{
		tPayloadTWRData Pld;
		//tPayloadTWR::value_type Pld; // GCC: "'struct utils::packet_TWR::tPayloadTWR utils::packet_TWR::tPayloadTWR::tPayloadTWR' is inaccessible within this context"
		Pld.MsgId = id;
		Pld.MsgStatus = status;
		Pld.Endpoint = ep;
		Pld.Payload = msgData;
		return tPacketTWRRsp(Pld);
	}
};

}
}
