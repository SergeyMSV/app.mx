#include "main.h"
#include "main_server.h"

#include <devDataSetConfig.h>

#include <utilsBase.h>
#include <utilsException.h>
#include <utilsExits.h>
#include <utilsPath.h>

#include <sharePortSPI.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

tTWRQueue TWRQueue; // [TBD] it is to be TWRQueueIn

#ifdef UDP_SERVER_TEST
void UDP_ClientTest(std::uint16_t port);
#endif // UDP_SERVER_TEST

#ifdef UDP_SERVER_TEST
void ThreadPortDEMO(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server);
#endif // UDP_SERVER_TEST
void ThreadPortSPI(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server, tTWRQueueSPICmd& queueIn);
void ThreadDALLAS(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server);
int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		const std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		const std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

		std::shared_ptr<dev::tDataSetConfig> DsConfig = std::make_shared<dev::tDataSetConfig>(PathFileConfig, PathFileMX);
		if (DsConfig->GetUDPPort().IsWrong())
			THROW_RUNTIME_ERROR("IP-port is not valid");

		boost::asio::io_context ioc;
		tTWRServer Server(ioc, DsConfig->GetUDPPort().Value);
		std::thread Thread_ioc([&]() { ioc.run(); });

		// [!] Interfaces which are not needed for TWR are free when their configs are commented.

		// Each port must be in its own separate thread.
#ifdef UDP_SERVER_TEST
		std::thread Thread_DEMO([&DsConfig, &Server]() { ThreadPortDEMO(DsConfig, Server); });
#endif // UDP_SERVER_TEST
		std::thread Thread_SPI0_CS0([&DsConfig, &Server]() { ThreadPortSPI(DsConfig, Server, TWRQueue.SPI0_CS0); });
		std::thread Thread_DALLAS([&DsConfig, &Server]() { ThreadDALLAS(DsConfig, Server); });

#ifdef UDP_SERVER_TEST
		UDP_ClientTest(DsConfig->GetUDPPort().Value);
#else // UDP_SERVER_TEST
		while (true) // [TBD] It must be stopped if the application is being terminated.
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//std::this_thread::yield();
		}
#endif // UDP_SERVER_TEST

		DsConfig.reset(); // All threads will notice it and stop. In other words it stops all the threads.

		ioc.stop();
		Thread_ioc.join();
#ifdef UDP_SERVER_TEST
		Thread_DEMO.join();
#endif // UDP_SERVER_TEST
		Thread_SPI0_CS0.join();
		Thread_DALLAS.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::exit_code::EX_IOERR);
	}

	return static_cast<int>(utils::exit_code::EX_OK);
}
