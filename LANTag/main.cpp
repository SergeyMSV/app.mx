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
		std::shared_ptr<dev::tConfig> config = std::make_shared<dev::tConfig>(argv[0]);
		
		boost::asio::io_context ioc;

		dev::tLANTagServer Server(ioc, config->GetPortUDP().Value, config);

		std::thread iocThread([&]() { ioc.run(); });

		//DsConfig.reset();

		while (true) // [TBD] It must be stopped if the application is being terminated.
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//std::this_thread::yield();
		}

		ioc.stop();
		iocThread.join();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return utils::exit_code::EX_IOERR;
	}

	return utils::exit_code::EX_OK;
}
