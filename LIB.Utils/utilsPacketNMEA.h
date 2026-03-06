///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketNMEA
// 2019-01-31
// C++17
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <libConfig.h>

#include "utilsBase.h"
#include "utilsCRC.h"

#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace utils
{
namespace packet
{
namespace nmea
{
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class TPayload, std::uint8_t stx = '$'>
struct tFormat
{
	enum : std::uint8_t { STX = stx };

	// The maximum number of characters in a sentence shall be 82, consisting of a maximum of 79 characters between the starting delimiter "$" or "!" and the terminating <CR><LF>.
	static constexpr std::size_t m_MaxPacketSize = 100;

protected:
	static std::optional<TPayload> Parse(const std::vector<std::uint8_t>& data, std::size_t& bytesToRemove)
	{
		const auto PosETX = std::find(data.begin(), data.end(), '\xa');
		if (PosETX == data.end()) // Whole Packet hasn't been received yet (partly received).
		{
			if (data.size() > m_MaxPacketSize)
				bytesToRemove = data.size() - m_MaxPacketSize;
			return {};
		}
		bytesToRemove = std::distance(data.begin(), PosETX) + 1; // +1 for ETX
		auto PosSTX = FindReverse(data.begin(), PosETX, STX);
		const std::size_t PacketSize = std::distance(PosSTX, PosETX);
		if (PacketSize < GetSize(0) || *(PosETX - 4) != '*')
			return {};
		auto PayloadBeg = PosSTX + 1; // $*xx\xd\xa
		auto PayloadEnd = PosETX - 4; // $*xx\xd\xa
		std::uint8_t CRC = utils::crc::CRC08_NMEA(PayloadBeg, PayloadEnd);
		std::uint8_t CRCPack = static_cast<std::uint8_t>(std::strtoul(reinterpret_cast<const char*>(&(*(PayloadEnd + 1))), nullptr, 16));
		if (CRC != CRCPack)
		{
			PosSTX = std::find(PosSTX + 1, data.end(), STX); // That's for parsing damaged packets, something like that: "GNGG$GNGG$GNGGA,221$GPGSV,3,1,10,23,...".
			bytesToRemove = std::distance(data.begin(), PosSTX);
			return {};
		}
		return TPayload(PayloadBeg, PayloadEnd);
	}

	static std::optional<TPayload> Parse(const std::vector<std::uint8_t>& data)
	{
		std::size_t BytesToRemove;
		return Parse(data, BytesToRemove);
	}

	static std::size_t GetSize(std::size_t payloadSize) { return payloadSize + 6; }; // $*xx\xd\xa

	static void Append(std::vector<std::uint8_t>& dst, const TPayload& payload)
	{
		if (payload.empty())
			return;
		dst.reserve(GetSize(payload.size()));
		dst.push_back(STX);
		for (auto i : payload)
		{
			dst.push_back(i);
		}
		dst.push_back('*');

		const std::uint8_t CRC = utils::crc::CRC08_NMEA(payload.begin(), payload.end());

		std::stringstream SStr;
		SStr << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<uint16_t>(CRC);
		SStr << "\xd\xa";
		const std::string EndStr = SStr.str();
		dst.insert(dst.end(), EndStr.cbegin(), EndStr.cend());
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class TPayload>
using tFormatNMEA = tFormat<TPayload>;

template <class TPayload>
using tFormatAIS = tFormat<TPayload, '!'>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class TPayload, std::uint8_t stx = '$'>
struct tFormatNMEANoCRC
{
	enum : std::uint8_t { STX = stx };

	// The maximum number of characters in a sentence shall be 82, consisting of a maximum of 79 characters between the starting delimiter "$" or "!" and the terminating <CR><LF>.
	static constexpr std::size_t m_MaxPacketSize = 100;

protected:
	static std::optional<TPayload> Parse(const std::vector<std::uint8_t>& data, std::size_t& bytesToRemove)
	{
		const auto PosETX = std::find(data.begin(), data.end(), '\xa');
		if (PosETX == data.end()) // Whole Packet hasn't been received yet (partly received).
		{
			if (data.size() > m_MaxPacketSize)
				bytesToRemove = data.size() - m_MaxPacketSize;
			return {};
		}
		bytesToRemove = std::distance(data.begin(), PosETX) + 1; // +1 for ETX
		const auto PosSTX = FindReverse(data.begin(), PosETX, STX);
		const std::size_t PacketSize = std::distance(PosSTX, PosETX);
		if (PacketSize < GetSize(0))
			return {};
		// If a packet doesn't contain CRC - that's OK, but if the packet contains CRC, that CRC must be right.
		auto PayloadBeg = PosSTX + 1; // $\xd\xa
		auto PayloadEnd = PosETX - 1; // $\xd\xa
		if (PacketSize > 5 && *(PosETX - 4) == '*') // $*xx\xd\xa - If the packet contains CRC last field shouldn't contain that CRC.
		{
			PayloadEnd = PosETX - 4;
			std::uint8_t CRC = utils::crc::CRC08_NMEA(PayloadBeg, PayloadEnd);
			std::uint8_t CRCPack = static_cast<std::uint8_t>(std::strtoul(reinterpret_cast<const char*>(&(*(PayloadEnd + 1))), nullptr, 16));
			if (CRC != CRCPack)
			{
				const auto PosSTX2 = std::find(PosSTX + 1, data.end(), STX); // That's for parsing damaged packets, something like that: "GNGG$GNGG$GNGGA,221$GPGSV,3,1,10,23,...".
				bytesToRemove = std::distance(data.begin(), PosSTX2);
				return {};
			}
		}
		return TPayload(PayloadBeg, PayloadEnd);
	}

	static std::size_t GetSize(std::size_t payloadSize) { return payloadSize + 3; }; // $\xd\xa

	static void Append(std::vector<std::uint8_t>& dst, const TPayload& payload)
	{
		dst.reserve(GetSize(payload.size()));
		dst.push_back(STX);
		for (auto i : payload)
		{
			dst.push_back(i);
		}
		dst.push_back('\xd');
		dst.push_back('\xa');
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tPayloadCommon
{
	typedef std::vector<std::string> value_type;

	class tIterator
	{
		friend struct tPayloadCommon;

		const tPayloadCommon* m_pObj = nullptr;

		const std::size_t m_DataSize = 0;
		std::size_t m_DataIndex = 0;
		const char* m_DataPtr = nullptr;

		tIterator() = delete;
		tIterator(const tPayloadCommon* obj, bool begin) :m_pObj(obj), m_DataSize(m_pObj->size())
		{
			if (!m_DataSize || !m_pObj->Value[0].size())
				return;

			if (begin)
			{
				m_DataPtr = &m_pObj->Value[0][0];
			}
			else
			{
				m_DataIndex = m_DataSize;
			}
		}

	public:
		tIterator& operator ++ ()
		{
			if (m_DataIndex < m_DataSize)
				++m_DataIndex;

			std::size_t DataIndex = m_DataIndex;

			for (const std::string& i : m_pObj->Value)
			{
				const std::size_t StrSize = i.size();

				if (DataIndex >= StrSize + 1)
				{
					DataIndex -= StrSize + 1;
				}
				else if (DataIndex == StrSize)
				{
					static char Separator = ',';
					m_DataPtr = &Separator;
					break;
				}
				else
				{
					m_DataPtr = &i[DataIndex];
					break;
				}
			}

			return *this;
		}

		bool operator != (const tIterator& val) const
		{
			return m_DataIndex != val.m_DataIndex;
		}

		const char operator * () const
		{
			assert(m_DataPtr != nullptr);

			return *m_DataPtr;
		}
	};

	typedef tIterator iterator;

	value_type Value{};

	tPayloadCommon() = default;
	explicit tPayloadCommon(const value_type& value) :Value(value) {}
	tPayloadCommon(std::vector<std::uint8_t>::const_iterator cbegin, std::vector<std::uint8_t>::const_iterator cend)
	{
		AppendData(cbegin, cend);
	}
	tPayloadCommon(std::string::const_iterator cbegin, std::string::const_iterator cend)
	{
		AppendData(cbegin, cend);
	}

	bool empty() const { return Value.empty(); }

	std::size_t size() const
	{
		std::size_t Size = 0;

		for (const auto& i : Value)
		{
			Size += i.size();
		}

		if (Value.size() > 0) // that's for ','
			Size += Value.size() - 1;

		return Size;
	}

	iterator begin() const { return iterator(this, true); }
	iterator end() const { return iterator(this, false); }

private:
	template <class It>
	void AppendData(It begin, It end)
	{
		std::string Str;

		for (It i = begin; i != end; ++i)
		{
			if (*i == ',')
			{
				Value.push_back(Str);
				Str.clear();
			}
			else
			{
				Str.push_back(static_cast<char>(*i));
			}
		}

		Value.push_back(Str);
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tPayloadString
{
	typedef std::string value_type;
	typedef value_type::const_iterator iterator;

	value_type Value{};

	tPayloadString() = default;
	explicit tPayloadString(const value_type& value) :Value(value) {}
	tPayloadString(std::vector<std::uint8_t>::const_iterator cbegin, std::vector<std::uint8_t>::const_iterator cend)
	{
		Value.insert(Value.end(), cbegin, cend);
	}

	bool empty() const { return Value.empty(); }
	std::size_t size() const { return Value.size(); }
	iterator begin() const { return Value.begin(); }
	iterator end() const { return Value.end(); }
};
///////////////////////////////////////////////////////////////////////////////////////////////////
using tPacketNMEA_Common_CRC = utils::packet::tPacket<tFormatNMEA, utils::packet::nmea::tPayloadCommon>;
using tPacketNMEA_Common_NoCRC = utils::packet::tPacket<tFormatNMEANoCRC, utils::packet::nmea::tPayloadCommon>;
using tPacketNMEA_String_CRC = utils::packet::tPacket<tFormatNMEA, utils::packet::nmea::tPayloadString>;
using tPacketNMEA_String_NoCRC = utils::packet::tPacket<tFormatNMEANoCRC, utils::packet::nmea::tPayloadString>;
///////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
