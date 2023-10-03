///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketStar
// 2015-07-17
// Standard ISO/IEC 114882, C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsCRC.h"
#include "utilsPacket.h"

namespace utils
{
namespace packet_Star
{

template <class TPayload>
struct tFormatStar
{
	typedef std::uint16_t tFieldDataSize;

	enum : std::uint8_t { STX = '*' };

protected:
	static tVectorUInt8 TestPacket(tVectorUInt8::const_iterator cbegin, tVectorUInt8::const_iterator cend)
	{
		const std::size_t Size = std::distance(cbegin, cend);

		if (Size >= GetSize(0) && *cbegin == STX)
		{
			tFieldDataSize DataSize = 0;

			const tVectorUInt8::const_iterator Begin = cbegin + 1;
			const tVectorUInt8::const_iterator End = Begin + sizeof(tFieldDataSize);

			std::copy(Begin, End, reinterpret_cast<std::uint8_t*>(&DataSize));

			if (Size >= GetSize(DataSize) && VerifyCRC(Begin, sizeof(tFieldDataSize) + DataSize))
			{
				return tVectorUInt8(cbegin, cbegin + GetSize(DataSize));
			}
		}

		return {};
	}

	static bool TryParse(const tVectorUInt8& packetVector, TPayload& payload)
	{
		if (packetVector.size() >= GetSize(0) && packetVector[0] == STX)
		{
			tFieldDataSize DataSize = 0;

			tVectorUInt8::const_iterator Begin = packetVector.cbegin() + 1;
			tVectorUInt8::const_iterator End = Begin + sizeof(tFieldDataSize);

			std::copy(Begin, End, reinterpret_cast<std::uint8_t*>(&DataSize));

			if (packetVector.size() == GetSize(DataSize) && VerifyCRC(Begin, sizeof(tFieldDataSize) + DataSize))
			{
				Begin = End;
				End += DataSize;

				payload = TPayload(Begin, End);

				return true;
			}
		}

		return false;
	}

	static std::size_t GetSize(std::size_t payloadSize) { return sizeof(STX) + sizeof(tFieldDataSize) + payloadSize + 2; }//2 - for CRC

	void Append(tVectorUInt8& dst, const TPayload& payload) const
	{
		dst.reserve(GetSize(payload.size()));

		dst.push_back(STX);

		utils::Append(dst, static_cast<std::uint16_t>(payload.size()));

		for (const auto i : payload)
		{
			dst.push_back(i);
		}

		const std::size_t DataSize = sizeof(tFieldDataSize) + payload.size();

		const std::uint16_t CRC = utils::crc::CRC16_CCITT(dst.end() - DataSize, dst.end());

		utils::Append(dst, CRC);
	}

private:
	static bool VerifyCRC(tVectorUInt8::const_iterator begin, std::size_t crcDataSize)
	{
		const std::uint16_t CRC = utils::crc::CRC16_CCITT(begin, begin + crcDataSize);

		const tVectorUInt8::const_iterator CRCBegin = begin + crcDataSize;

		const std::uint16_t CRCReceived = utils::Read<std::uint16_t>(CRCBegin, CRCBegin + sizeof(CRC));

		return CRC == CRCReceived;
	}
};

namespace example
{
// Payload: tVectorUInt8
// [STX='*' 1-Byte][PayloadSize 2-Bytes LittleEndian][Payload, its size can be up to 1024-Bytes][CRC16 CCITT 2-Bytes (PayloadSize and Payload, except STX) LittleEndian]

// Payload: tPayload<tDataCmd>
// [STX='*' 1-Byte][PayloadSize 2-Bytes LittleEndian][MsgId 1-Byte][Payload, its size can be up to 1023-Bytes][CRC16 CCITT 2-Bytes (PayloadSize and Payload, except STX) LittleEndian]


// [Payload, its size can be up to 1024-Bytes]
using tPacketSimple = packet::tPacket<tFormatStar, tVectorUInt8>;

// [MsgId 1-Byte][Payload, its size can be up to 1023-Bytes]
struct tPayloadMsgData
{
	std::uint8_t MsgId = 0;
	tVectorUInt8 Payload;

	tPayloadMsgData() = default;
	tPayloadMsgData(std::uint8_t msgId, const tVectorUInt8& payload)
		:MsgId(msgId), Payload(payload)
	{}

	tPayloadMsgData(tVectorUInt8::const_iterator cbegin, tVectorUInt8::const_iterator cend)
	{
		const std::size_t DataSize = std::distance(cbegin, cend);
		if (DataSize < 1)
			return;

		MsgId = *cbegin++;
		Payload = tVectorUInt8(cbegin, cend);
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

struct tPayloadMsg : public packet::tPayload<tPayloadMsgData>
{
	tPayloadMsg() = default;
	tPayloadMsg(tVectorUInt8::const_iterator cbegin, tVectorUInt8::const_iterator cend)
		:tPayload(cbegin, cend)
	{}
};

class tPacketMsg : public packet::tPacket<tFormatStar, tPayloadMsg>
{
	explicit tPacketMsg(const payload_value_type& payloadValue)
		:tPacket(payloadValue)
	{}

public:
	tPacketMsg() = default;

	static tPacketMsg MakeSomeMessage_01(const tVectorUInt8& msgData)
	{
		tPayloadMsgData Pld;
		//tPayloadTWR::value_type Pld; // GCC: "'struct utils::packet_TWR::tPayloadTWR utils::packet_TWR::tPayloadTWR::tPayloadTWR' is inaccessible within this context"
		Pld.MsgId = 0x01;
		Pld.Payload = msgData;
		return tPacketMsg(Pld);
	}
	// ... Make-functions for other packets
};

}
}
}
