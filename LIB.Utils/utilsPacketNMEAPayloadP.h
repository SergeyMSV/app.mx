///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketNMEAPayloadP
// 2026-02-22
// C++17
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsPacketNMEA.h"
#include "utilsPacketNMEAType.h"

namespace utils
{
namespace packet
{
namespace nmea
{
namespace base
{
///////////////////////////////////////////////////////////////////////////////////////////////////
template <int... ints>
struct tContentP : public type::tTypeVerified // 'P' stands for proprietary
{
	std::vector<std::string> Value;

private:
	int m_HeaderParts = 0;

public:
	tContentP() = default;
	explicit tContentP(const std::vector<std::string>& val)
	{
		if (val.size() < 1)
		{
			SetVerified(false);
			return;
		}
		int Position = 0;
		((!Parse(val[0], ints, Position)) || ...);
		for (std::size_t i = 1; i < val.size(); ++i)
		{
			Value.push_back(val[i]);
		}
	}

	std::vector<std::string> GetPayload() const
	{
		std::string Header;
		for (std::size_t i = 0; i < m_HeaderParts && i < Value.size(); ++i)
		{
			Header += Value[i];
		}
		std::vector<std::string> Data;
		Data.push_back(Header);
		for (std::size_t i = m_HeaderParts; i < Value.size(); ++i)
		{
			Data.push_back(Value[i]);
		}
		return Data;
	}

private:
	bool Parse(const std::string& values, int size, int& position)
	{
		if (position >= values.size())
			return false;
		Value.push_back(values.substr(position, size));
		position += size;
		++m_HeaderParts;
		return true;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
}
