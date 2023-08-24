#pragma once

#include <utilsBase.h>

#include <atomic>
#include <mutex>
#include <string>

struct tDataSetMainControl
{
	enum class tStateGNSS
	{
		Nothing,
		Halt,
		Start,
		Restart,
		Exit,
		UserTaskScriptStart,
	};

	std::atomic<tStateGNSS> Thread_GNSS_State{ tStateGNSS::Halt };

	mutable std::mutex Thread_GNSS_State_UserTaskScriptIDMtx;
	std::string Thread_GNSS_State_UserTaskScriptID;
};

constexpr char g_FileNameTempPrefix[] = "_";

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif // _WIN32