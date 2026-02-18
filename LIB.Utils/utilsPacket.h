///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacket
// 2019-06-20
// C++17
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <libConfig.h>

#include <algorithm>
#include <optional>
#include <vector>

#include <cstdint>

namespace utils
{
namespace packet
{

template
<
	template <class> class TFormat,
	class TPayload
>
class tPacket : private TFormat<TPayload>, public TPayload
{
public:
	typedef TPayload payload_type;
	typedef typename TPayload::value_type payload_value_type;

	tPacket() = default;

	explicit tPacket(const TPayload& payload) = delete;

	explicit tPacket(TPayload&& payload)
	{
		*static_cast<TPayload*>(this) = std::move(payload);
	}

	explicit tPacket(const payload_value_type& payloadValue) = delete;

	explicit tPacket(payload_value_type&& payloadValue)
	{
		TPayload::Value = std::move(payloadValue);
	}

	static std::optional<tPacket> Find(std::vector<std::uint8_t>& data)
	{
		while (true)
		{
			if (data.empty())
				break;
			std::size_t BytesToRemove = 0;
			std::optional<TPayload> PacketOpt = TFormat<TPayload>::Parse(data, BytesToRemove);
			data.erase(data.begin(), data.begin() + BytesToRemove);
			if (PacketOpt.has_value())
				return tPacket(std::move(*PacketOpt));
			if (!BytesToRemove)
				break;
		}
		return {};
	}

	const payload_value_type& GetPayloadValue() const
	{
		return TPayload::Value;
	}

	std::vector<std::uint8_t> ToVector() const
	{
		std::vector<std::uint8_t> Data;
		TFormat<TPayload>::Append(Data, *this);
		return Data;
	}
};

template <class TValue>
struct tPayload
{
	typedef TValue value_type;

	class tIterator
	{
		friend struct tPayload;

		const tPayload* m_pObj = nullptr;

		const std::size_t m_DataSize = 0;
		std::size_t m_DataIndex = 0;

		tIterator() = delete;
		tIterator(const tPayload* obj, bool begin)
			:m_pObj(obj), m_DataSize(m_pObj->size())
		{
			if (!m_DataSize)
				return;

			m_DataIndex = begin ? 0 : m_DataSize;
		}

	public:
		tIterator& operator ++ ()
		{
			if (m_DataIndex < m_DataSize)
				++m_DataIndex;
			return *this;
		}

		bool operator != (const tIterator& val) const
		{
			return m_DataIndex != val.m_DataIndex;
		}

		const std::uint8_t operator * () const
		{
			return m_pObj->Value[m_DataIndex];
		}
	};

	typedef tIterator iterator;

	value_type Value{};

	tPayload() = default;
	tPayload(std::vector<std::uint8_t>::const_iterator cbegin, std::vector<std::uint8_t>::const_iterator cend)
		:Value(cbegin, cend)
	{}

	bool empty() const { return Value.empty(); }
	std::size_t size() const { return Value.size(); }
	iterator begin() const { return iterator(this, true); }
	iterator end() const { return iterator(this, false); }
};

}
}
