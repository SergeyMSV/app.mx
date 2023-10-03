#pragma once
#include <utilsBase.h>

using tVectorUInt8 = utils::tVectorUInt8;

constexpr char Version[] = "TWR.0.1.1";

#define MXTWR_SERVER

#ifdef _WIN32
#define UDP_SERVER_TEST
#define _WIN32_WINNT 0x0601
#endif // _WIN32