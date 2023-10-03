#include "main.h"
#include "main_server.h"

#include <devDataSetConfig.h>

#include <utilsBase.h>
#include <utilsPath.h>

#include <sharePortSPI.h>

#include <array>
#include <iostream>
#include <string>

tTWRQueue TWRQueue;

#ifdef UDP_SERVER_TEST
void UDP_ClientTest(std::uint16_t port);
#endif // UDP_SERVER_TEST

void Thread_Port_DEMO(tTWRServer& twrServer, tTWRQueueDEMOCmd& twrQueue);
void Thread_Port_SPI(const share_config::tSPIPort& confSPI, const share_config::tGPIOPort& confRST, tTWRServer& twrServer, tTWRQueueSPICmd& twrQueue);

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

		dev::tDataSetConfig DsConfig(PathFileConfig, PathFileMX);
		
		boost::asio::io_context ioc;
		tTWRServer Server(ioc, DsConfig.GetUDPServer().Port);
		std::thread Thread_ioc([&]() { ioc.run(); });

		// Each port must be in its own separate thread.
		std::thread Thread_DEMO([&Server]() { Thread_Port_DEMO(Server, TWRQueue.DEMO); });
		std::thread Thread_SPI0_0([&DsConfig, &Server]() { Thread_Port_SPI(DsConfig.GetSPI_0_0(), DsConfig.GetSPI_0_0_RST(), Server, TWRQueue.SPI_0_0); });

#ifdef UDP_SERVER_TEST
		UDP_ClientTest(DsConfig.GetUDPServer().Port);
#else
		while (true) // [TBD] It must be stopped if the application is being terminated.
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//std::this_thread::yield();
		}
#endif // UDP_SERVER_TEST

		ioc.stop();
		Thread_ioc.join();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::tExitCode::EX_IOERR);
		//return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
