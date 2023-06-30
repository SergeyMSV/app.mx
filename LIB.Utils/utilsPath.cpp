#include "utilsPath.h"

#include <filesystem>
#include <iomanip>
#include <sstream>

namespace utils
{

std::string GetDateTime(tm a_DateTime)
{
	std::ostringstream oss;
	oss << std::put_time(&a_DateTime, "%Y-%m-%d_%H-%M-%S");
	return oss.str();
}

std::string GetDateTime()
{
	time_t TimeNow = std::time(nullptr);
	tm* Time = std::localtime(&TimeNow);
	return GetDateTime(*Time);
}

tm GetDateTime(const std::string& a_value)
{
	tm DateTime{};
	std::istringstream iss(a_value);
	iss >> std::get_time(&DateTime, "%Y-%m-%d_%H-%M-%S");
	return DateTime;
}

std::string GetAppName(const std::string& path)
{
	const std::filesystem::path Path{ path };
	std::filesystem::path PathFileName = Path.filename();
	if (PathFileName.has_extension())
		PathFileName.replace_extension();
	return PathFileName.string();
}

std::string GetAppNameMain(const std::string& path)
{
	std::string MainPart = GetAppName(path);
	// Main part of application name: mfrc522_xxx
	std::size_t Pos = MainPart.find_last_of('_');
	if (Pos != std::string::npos)
		MainPart = MainPart.substr(0, Pos);
	return MainPart;
}

}
