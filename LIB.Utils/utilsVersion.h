///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsVersion
// 2024-02-04 
// C++14
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <string>

namespace utils
{

struct tVersion // 1.0.234
{
	std::uint16_t Major = 0;
	std::uint16_t Minor = 0;
	std::uint16_t Build = 0;

	tVersion() = default;
	tVersion(std::uint16_t major, std::uint16_t minor, std::uint16_t build);
	explicit tVersion(const std::string& strVersion);

	bool operator==(const tVersion& val) const;
	bool operator!=(const tVersion& val) const;
	//bool operator==(const tVersion&)const = default;//[TBD] - C++20 set at the beginning of the file
	//bool operator!=(const tVersion&)const = default;

	bool operator<(const tVersion& val) const;
	bool operator>(const tVersion& val) const;

	static bool TryParse(const std::string& strVersion, tVersion& version);

	std::string ToString() const;
};

}
