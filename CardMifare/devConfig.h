#pragma once
#include <utilsBase.h>

using tVectorUInt8 = utils::tVectorUInt8;

//constexpr char Version[] = "TWR.0.1.1";

#define MXTWR_CLIENT
#define MXTWR_PORT 50000
constexpr std::string_view Host{ "192.168.10.161" };

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif // _WIN32