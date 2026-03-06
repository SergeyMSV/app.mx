///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsString
// 2026-03-03
// C++17
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace utils
{
namespace string
{

std::string_view TrimString(std::string_view str);
std::vector<std::string_view> SplitString(std::string_view str, char delimiter);
std::vector<std::string_view> SplitTrimString(std::string_view str, char delimiter);
bool Contains(std::string_view str, std::string_view substr);

}
}
