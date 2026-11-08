#include "dev/rtc.h"

#include <utilsExits.h>
#include <thread>

int main()
{
	std::thread rtcThread([]() { dev::ThreadRTC(); });

	rtcThread.join();

	return utils::exit_code::EX_OK;
}
