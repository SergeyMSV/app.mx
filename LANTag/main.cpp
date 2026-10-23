#include "main.h"
#include "dev/config.h"
#include "dev/server.h"

#include <utilsExits.h>

#include <iostream>
#include <memory>
#include <string>

int main(int argc, char* argv[])
{
	try
	{
		std::shared_ptr<dev::tConfig> DsConfig = std::make_shared<dev::tConfig>();
		
		boost::asio::io_context ioc;

		dev::tLANTagServer Server(ioc, DsConfig->GetPortUDP().Value, DsConfig);

		std::thread Thread_ioc([&]() { ioc.run(); });

		//DsConfig.reset();

		while (true) // [TBD] It must be stopped if the application is being terminated.
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//std::this_thread::yield();
		}

		ioc.stop();
		Thread_ioc.join();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::exit_code::EX_IOERR);
	}

	return static_cast<int>(utils::exit_code::EX_OK);
}
