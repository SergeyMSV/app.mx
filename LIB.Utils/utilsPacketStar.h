///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketStar
// 2015-07-17
// C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsBase.h"
#include "utilsCRC.h"
#include "utilsPacket.h"

namespace utils
{
namespace packet
{
namespace star
{

template <class TPayload>
struct tFormatStar
{
	typedef std::uint16_t tFieldDataSize;

	enum : std::uint8_t { STX = '*' };

protected:
	static std::vector<std::uint8_t> TestPacket(std::vector<std::uint8_t>::const_iterator cbegin, std::vector<std::uint8_t>::const_iterator cend)
	{
		const std::size_t Size = std::distance(cbegin, cend);

		if (Size < GetSize(0) || *cbegin != STX)
			return {};

		tFieldDataSize DataSize = 0;

		const std::vector<std::uint8_t>::const_iterator Begin = cbegin + 1;
		const std::vector<std::uint8_t>::const_iterator End = Begin + sizeof(tFieldDataSize);

		std::copy(Begin, End, reinterpret_cast<std::uint8_t*>(&DataSize));

		if (Size < GetSize(DataSize) || !VerifyCRC(Begin, sizeof(tFieldDataSize) + DataSize))
			return {};

		return std::vector<std::uint8_t>(cbegin, cbegin + GetSize(DataSize));
	}

	static bool TryParse(const std::vector<std::uint8_t>& packetVector, TPayload& payload)
	{
		if (packetVector.size() < GetSize(0) || packetVector[0] != STX)
			return false;

		tFieldDataSize DataSize = 0;

		std::vector<std::uint8_t>::const_iterator Begin = packetVector.cbegin() + 1;
		std::vector<std::uint8_t>::const_iterator End = Begin + sizeof(tFieldDataSize);

		std::copy(Begin, End, reinterpret_cast<std::uint8_t*>(&DataSize));

		if (packetVector.size() != GetSize(DataSize) || !VerifyCRC(Begin, sizeof(tFieldDataSize) + DataSize))
			return false;

		Begin = End;
		End += DataSize;

		payload = TPayload(Begin, End);

		return true;
	}

	static std::size_t GetSize(std::size_t payloadSize) { return sizeof(STX) + sizeof(tFieldDataSize) + payloadSize + 2; } // 2 - for CRC

	void Append(std::vector<std::uint8_t>& dst, const TPayload& payload) const
	{
		dst.reserve(GetSize(payload.size()));

		dst.push_back(STX);

		utils::Append(dst, static_cast<std::uint16_t>(payload.size()));

		for (const auto i : payload)
		{
			dst.push_back(i);
		}

		std::size_t DataSize = sizeof(tFieldDataSize) + payload.size();

		std::uint16_t CRC = utils::crc::CRC16_CCITT(dst.end() - DataSize, dst.end());

		utils::Append(dst, CRC);
	}

private:
	static bool VerifyCRC(std::vector<std::uint8_t>::const_iterator begin, std::size_t crcDataSize)
	{
		std::uint16_t CRC = utils::crc::CRC16_CCITT(begin, begin + crcDataSize);

		std::vector<std::uint8_t>::const_iterator CRCBegin = begin + crcDataSize;

		std::uint16_t CRCReceived = utils::Read<std::uint16_t>(CRCBegin, CRCBegin + sizeof(CRC));

		return CRC == CRCReceived;
	}
};

namespace example
{
// Payload: std::vector<std::uint8_t>
// [STX='*' 1-Byte][PayloadSize 2-Bytes LittleEndian][Payload, its size can be up to 1024-Bytes][CRC16 CCITT 2-Bytes (PayloadSize and Payload, except STX) LittleEndian]

// Payload: tPayload<tDataCmd>
// [STX='*' 1-Byte][PayloadSize 2-Bytes LittleEndian][MsgId 1-Byte][Payload, its size can be up to 1023-Bytes][CRC16 CCITT 2-Bytes (PayloadSize and Payload, except STX) LittleEndian]


// [Payload, its size can be up to 1024-Bytes]
using tPacketSimple = utils::packet::tPacket<tFormatStar, std::vector<std::uint8_t>>;

// [MsgId 1-Byte][Payload, its size can be up to 1023-Bytes]
struct tPayloadMsgData
{
	std::uint8_t MsgId = 0;
	std::vector<std::uint8_t> Payload;

	tPayloadMsgData() = default;
	tPayloadMsgData(std::uint8_t msgId, const std::vector<std::uint8_t>& payload)
		:MsgId(msgId), Payload(payload)
	{}

	tPayloadMsgData(std::vector<std::uint8_t>::const_iterator cbegin, std::vector<std::uint8_t>::const_iterator cend)
	{
		const std::size_t DataSize = std::distance(cbegin, cend);
		if (DataSize < 1)
			return;

		MsgId = *cbegin++;
		Payload = std::vector<std::uint8_t>(cbegin, cend);
	}

	std::size_t size() const
	{
		return 1 + Payload.size();
	}

	std::uint8_t operator[] (const std::size_t index) const
	{
		if (index >= size())
			return 0;

		switch (index)
		{
		case 0: return MsgId;
		}
		return Payload[index - 1];
	}

	bool operator == (const tPayloadMsgData& val) const = default;
	bool operator != (const tPayloadMsgData& val) const = default;
};

struct tPayloadMsg : public utils::packet::tPayload<tPayloadMsgData>
{
	tPayloadMsg() = default;
	tPayloadMsg(std::vector<std::uint8_t>::const_iterator cbegin, std::vector<std::uint8_t>::const_iterator cend)
		:tPayload(cbegin, cend)
	{}
};

class tPacketMsg : public utils::packet::tPacket<tFormatStar, tPayloadMsg>
{
	explicit tPacketMsg(const payload_value_type& payloadValue)
		:tPacket(payloadValue)
	{}

public:
	tPacketMsg() = default;

	static tPacketMsg MakeSomeMessage_01(const std::vector<std::uint8_t>& msgData)
	{
		payload_value_type Pld;
		Pld.MsgId = 0x01;
		Pld.Payload = msgData;
		return tPacketMsg(Pld);
	}
	// ... Make-functions for other packets
};

}
}
}
}
