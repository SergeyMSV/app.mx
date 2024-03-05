#pragma once
#include <utilsBase.h>

using tVectorUInt8 = utils::tVectorUInt8;

#ifdef _WIN32
#define UDP_SERVER_TEST

namespace settings
{
	constexpr std::string_view Network{ "192.168.10.0/24" };
}

#define _WIN32_WINNT 0x0601
#endif // _WIN32

#define BOOST_ASIO_NO_DEPRECATED
