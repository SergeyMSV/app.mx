#include <utilsExits.h>
#include <thread>

void ThreadRTC();

int main()
{
	std::thread Thread_RTC([]() { ThreadRTC(); });

	Thread_RTC.join();

	return utils::exit_code::EX_OK;
}
