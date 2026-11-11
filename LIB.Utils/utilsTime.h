///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsTime
// 2025-05-01 - 2026-11-03
// C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

#include <ctime>

namespace utils
{
namespace time
{

class tDateTime
{
	std::time_t Value = 0;

public:
	tDateTime() = default;
	tDateTime(std::time_t val) :Value(val) {} // it is not explicit

	static tDateTime Now();

	static tDateTime Parse(const std::string& value, const std::string& format);
	static tDateTime Parse(const std::string& value);
	static tDateTime ParsePath(const std::string& value);

	std::string ToString(std::string format) const;
	std::string ToString() const;
	std::string ToStringPath() const;
};

}
}
