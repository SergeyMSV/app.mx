#include "dev/rtc.h"

#include <utilsExits.h>
#include <thread>

int main()
{
	std::thread Thread_RTC([]() { dev::ThreadRTC(); });

	Thread_RTC.join();

	return utils::exit_code::EX_OK;
}
