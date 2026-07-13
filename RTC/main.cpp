#include <thread>

int main()
{
	while (true)
	{
		// [TBD] try to save the time
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	return 0;
}
