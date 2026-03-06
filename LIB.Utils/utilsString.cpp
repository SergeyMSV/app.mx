#include "utilsString.h"

namespace utils
{
namespace string
{

std::string_view TrimString(std::string_view str)
{
	std::size_t Start = 0;
	// Left Trim
	while (Start < str.size() && std::isspace(static_cast<unsigned char>(str[Start])))
	{
		++Start;
	}
	str = str.substr(Start);
	// Right Trim
	std::size_t End = str.size();
	while (End > 0 && std::isspace(static_cast<unsigned char>(str[End - 1])))
	{
		--End;
	}
	return str.substr(0, End);
}

std::vector<std::string_view> SplitString(std::string_view str, char delimiter)
{
	std::vector<std::string_view> Strs;

	std::size_t PosStart = 0;
	while (true)
	{
		std::size_t PosEnd = str.find(delimiter, PosStart);
		if (PosEnd == std::string_view::npos)
		{
			std::string_view Str = str.substr(PosStart);
			if (!Str.empty())
				Strs.push_back(Str);
			break;
		}
		Strs.push_back(str.substr(PosStart, PosEnd - PosStart));
		PosStart = PosEnd + 1;
	}
	return Strs;
}

std::vector<std::string_view> SplitTrimString(std::string_view str, char delimiter)
{
	std::vector<std::string_view> Strs = utils::string::SplitString(str, delimiter);
	for (std::size_t i = 0; i < Strs.size(); ++i)
	{
		Strs[i] = utils::string::TrimString(Strs[i]);
	}
	return Strs;
}

bool Contains(std::string_view str, std::string_view substr)
{
	return str.find(substr) != str.npos;
}

}
}
