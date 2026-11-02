#include "dev/client.h"
#include "dev/dallas.h"
#include "dev/port_demo.h"
#include "dev/port_spi.h"
#include "dev/port_uart.h"

#include <utilsException.h>
#include <utilsExits.h>

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
	try
	{
		std::shared_ptr<dev::tConfig> config = std::make_shared<dev::tConfig>(argv[0]);
		if (config->GetUDPPort().IsWrong())
			THROW_RUNTIME_ERROR("IP-port is not valid");

		boost::asio::io_context ioc;
		dev::tTWRServer server(ioc, config->GetUDPPort().Value);
		std::thread iocThread([&]() { ioc.run(); });

		// [!] Interfaces which are not needed for TWR are free when their configs are commented.

		// Each port must be in its own separate thread.
#ifdef UDP_SERVER_TEST
		std::thread demoThread([&config, &server]() { dev::ThreadPortDEMO(config, server); });
#endif // UDP_SERVER_TEST
		std::thread spi0cs0Thread([&config, &server]() { dev::ThreadPortSPI0_CS0(config, server); });
		std::thread dallasThread([&config, &server]() { dev::ThreadDALLAS(config, server); });
		std::thread uart0Thread([&config, &server]() { dev::ThreadUART0_JSON(config, server); });
		std::thread uart1Thread([&config, &server]() { dev::ThreadUART1_JSON(config, server); });
		std::thread uart2Thread([&config, &server]() { dev::ThreadUART2_JSON(config, server); });
		std::thread uart3Thread([&config, &server]() { dev::ThreadUART3_JSON(config, server); });

#ifdef UDP_SERVER_TEST
		dev::UDP_ClientTest(config->GetUDPPort().Value);
#else // UDP_SERVER_TEST
		while (true) // [TBD] It must be stopped if the application is being terminated.
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//std::this_thread::yield();
		}
#endif // UDP_SERVER_TEST

		config.reset(); // All threads will notice it and stop. In other words it stops all the threads.

		ioc.stop();
		iocThread.join();
#ifdef UDP_SERVER_TEST
		demoThread.join();
#endif // UDP_SERVER_TEST
		spi0cs0Thread.join();
		dallasThread.join();
		uart0Thread.join();
		uart1Thread.join();
		uart2Thread.join();
		uart3Thread.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return utils::exit_code::EX_IOERR;
	}

	return utils::exit_code::EX_OK;
}
