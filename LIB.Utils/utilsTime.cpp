#include "utilsTime.h"

#include <chrono>
#include <format>
#include <iomanip>
#include <sstream>

namespace utils
{
namespace time
{

constexpr char g_format_datetime[] = "%Y-%m-%d %H:%M:%S"; // "%F %T"; - date & time are not parsed with "%F %T" (2024-11-12 14:53:26)
constexpr char g_format_path[] = "%Y-%m-%d_%H-%M-%S";

tDateTime tDateTime::Now()
{
	const auto TimeNow = std::chrono::system_clock::now();
	return tDateTime{ std::chrono::system_clock::to_time_t(TimeNow) };
}

tDateTime tDateTime::Parse(const std::string& value, const std::string& format)
{
	std::chrono::sys_seconds tp;
	std::istringstream iss(value);
	iss >> std::chrono::parse(format, tp);
	if (iss.fail())
		return {};
	return std::chrono::system_clock::to_time_t(tp);
}

tDateTime tDateTime::Parse(const std::string& value)
{
	return Parse(value, g_format_datetime);
}

tDateTime tDateTime::ParsePath(const std::string& value)
{
	return Parse(value, g_format_path);
}

std::string tDateTime::ToString(std::string format) const
{
	std::tm tm;
#ifdef _WIN32
	gmtime_s(&tm, &Value);
#else // _WIN32
	gmtime_r(&Value, &tm);
#endif // _WIN32
	std::ostringstream oss;
	oss << std::put_time(&tm, format.c_str());
	return oss.str();
}

std::string tDateTime::ToString() const
{
	return ToString(g_format_datetime);
}

std::string tDateTime::ToStringPath() const
{
	return ToString(g_format_path);
}

}
}
