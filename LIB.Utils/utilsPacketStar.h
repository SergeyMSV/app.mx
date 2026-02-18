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
struct tFormat
{
	enum : std::uint8_t { STX = '*' };

private:
	static constexpr std::size_t m_MaxPacketSize = 1029; // [STX - 1][PayloadSize - 2][Payload - up to 1024][CRC16 CCITT - 2] = 1029 bytes

protected:
	static std::optional<TPayload> Parse(const std::vector<std::uint8_t>& data, std::size_t& bytesToRemove)
	{
		return Parse(data.begin(), data.end(), bytesToRemove);
	}

	static std::optional<TPayload> Parse(const std::vector<std::uint8_t>& data)
	{
		std::size_t BytesToRemove;
		return Parse(data.begin(), data.end(), BytesToRemove);
	}

	static std::size_t GetSize(std::size_t payloadSize) { return sizeof(STX) + 2 + payloadSize + 2; } // 2 for PayloadSize and 2 for CRC

	void Append(std::vector<std::uint8_t>& dst, const TPayload& payload) const
	{
		dst.reserve(GetSize(payload.size()));
		dst.push_back(STX);
		utils::Append(dst, static_cast<std::uint16_t>(payload.size()));
		for (const auto i : payload)
		{
			dst.push_back(i);
		}
		std::size_t DataSize = 2 + payload.size(); // 2 for PayloadSize
		std::uint16_t CRC = utils::crc::CRC16_CCITT(dst.end() - DataSize, dst.end());
		utils::Append(dst, CRC);
	}

protected:
	template<typename InputIt>
	static std::optional<TPayload> Parse(InputIt begin, InputIt end, std::size_t& bytesToRemove)
	{
		InputIt PosSTX = std::find(begin, end, STX);
		const std::size_t PacketSizePossible = std::distance(PosSTX, end);
		bytesToRemove = std::distance(begin, PosSTX);
		if (PacketSizePossible < GetSize(0))
			return{};
		InputIt SizeBeg = PosSTX + 1;
		InputIt SizeEnd = SizeBeg + 2; // 2 for PayloadSize
		std::uint16_t PayloadSize = 0;
		std::copy(SizeBeg, SizeEnd, reinterpret_cast<std::uint8_t*>(&PayloadSize));
		const std::size_t PacketSize = GetSize(PayloadSize);
		if (PacketSize <= PacketSizePossible && PacketSize <= m_MaxPacketSize)
		{
			std::size_t ContentSize = 2 + PayloadSize; // 2 for PayloadSize
			std::uint16_t CRC = utils::crc::CRC16_CCITT(SizeBeg, SizeBeg + ContentSize);
			std::uint16_t CRCPack = *(SizeBeg + ContentSize);
			CRCPack |= *(SizeBeg + ContentSize + 1) << 8;
			if (CRC == CRCPack)
			{
				bytesToRemove = std::distance(begin, PosSTX + GetSize(PayloadSize));
				InputIt PayloadBeg = SizeEnd;
				return TPayload(PayloadBeg, PayloadBeg + PayloadSize);
			}
		}
		const InputIt BeginNew = PosSTX + 1;
		std::size_t BytesToRemoveR = 0;
		std::optional<TPayload> PackOpt = Parse(BeginNew, end, BytesToRemoveR);
		if (!PackOpt.has_value())
			return {};
		bytesToRemove = std::distance(begin, BeginNew) + BytesToRemoveR;
		return PackOpt;
	}
};

namespace example
{
// Payload: std::vector<std::uint8_t>
// [STX='*' 1-Byte][PayloadSize 2-Bytes LittleEndian][Payload, its size can be up to 1024-Bytes][CRC16 CCITT 2-Bytes (PayloadSize and Payload, except STX) LittleEndian]

// Payload: tPayload<tDataCmd>
// [STX='*' 1-Byte][PayloadSize 2-Bytes LittleEndian][MsgId 1-Byte][Payload, its size can be up to 1023-Bytes][CRC16 CCITT 2-Bytes (PayloadSize and Payload, except STX) LittleEndian]


// [Payload, its size can be up to 1024-Bytes]
using tPacketSimple = utils::packet::tPacket<tFormat, std::vector<std::uint8_t>>;

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

	bool empty() const { return size() == 0; }

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

using tPacketMsgBase = utils::packet::tPacket<tFormat, tPayloadMsg>;

class tPacketMsg : public tPacketMsgBase
{
	explicit tPacketMsg(const payload_value_type& payloadValue) = delete;
	explicit tPacketMsg(payload_value_type&& payloadValue)
		:tPacket(std::move(payloadValue))
	{}

public:
	tPacketMsg() = default;
	explicit tPacketMsg(const tPayloadMsg& payload) = delete;
	explicit tPacketMsg(tPayloadMsg&& payload)
	{
		*static_cast<tPayloadMsg*>(this) = std::move(payload);
	}

	static std::optional<tPacketMsg> Find(std::vector<std::uint8_t>& data)
	{
		std::optional<tPacketMsgBase> PacketOpt = tPacketMsgBase::Find(data);
		if (!PacketOpt.has_value())
			return {};
		return tPacketMsg(std::move(*PacketOpt));
	}

	static tPacketMsg MakeSomeMessage_01(const std::vector<std::uint8_t>& msgData)
	{
		payload_value_type Pld;
		Pld.MsgId = 0x01;
		Pld.Payload = msgData;
		return tPacketMsg(std::move(Pld));
	}
	// ... Make-functions for other packets
};

}
}
}
}
