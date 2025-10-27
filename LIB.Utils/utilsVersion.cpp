#include "utilsVersion.h"

namespace utils
{

tVersion::tVersion(std::uint16_t major, std::uint16_t minor, std::uint16_t build)
	:Major(major), Minor(minor), Build(build)
{
}

tVersion::tVersion(const std::string& strVersion)
{
	if (!TryParse(strVersion, *this))
		*this = tVersion();
}

bool tVersion::operator==(const tVersion& val) const
{
	return Major == val.Major && Minor == val.Minor && Build == val.Build;
}

bool tVersion::operator!=(const tVersion& val) const
{
	return !operator==(val);
}

bool tVersion::operator<(const tVersion& val) const
{
	if (Major == val.Major)
	{
		if (Minor == val.Minor)
		{
			return Build < val.Build;
		}
		return Minor < val.Minor;
	}
	return Major < val.Major;
}

bool tVersion::operator>(const tVersion& val) const
{
	if (Major == val.Major)
	{
		if (Minor == val.Minor)
		{
			return Build > val.Build;
		}
		return Minor > val.Minor;
	}
	return Major > val.Major;
}

bool tVersion::TryParse(const std::string& strVersion, tVersion& version)
{
	version = tVersion{};

	auto IsNotVersionSymbol = [](char ch)->bool { return !isdigit(ch) && ch != '.'; };

	std::string Value = strVersion;
	Value.erase(std::remove_if(Value.begin(), Value.end(), IsNotVersionSymbol), Value.end());

	const std::size_t Part1Begin = 0;
	const std::size_t Part1End = Value.find('.');
	const std::size_t Part2Begin = Part1End + 1;
	const std::size_t Part2End = Value.find('.', Part2Begin);
	const std::size_t Part3Begin = Part2End + 1;
	const std::size_t Part3End = Value.size() - 1;

	if (Part1End == std::string::npos || Part2End == std::string::npos || Part2End == Value.size() - 1)
		return false;

	auto GetFigure = [&Value](std::size_t begin, std::size_t end)->long
	{
		std::string SubStr = Value.substr(begin, end);
		return std::strtol(SubStr.c_str(), nullptr, 10);
	};

	version.Major = static_cast<std::uint16_t>(GetFigure(Part1Begin, Part1End));
	version.Minor = static_cast<std::uint16_t>(GetFigure(Part2Begin, Part2End));
	version.Build = static_cast<std::uint16_t>(GetFigure(Part3Begin, Part3End));

	return true;
}

std::string tVersion::ToString() const
{
	return std::to_string(Major) + "." + std::to_string(Minor) + "." + std::to_string(Build);
}

}
