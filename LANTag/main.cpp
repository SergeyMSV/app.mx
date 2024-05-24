#include "main.h"
#include "main_server.h"

#include <devDataSetConfig.h>

#include <utilsBase.h>
#include <utilsPath.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

#ifdef UDP_SERVER_TEST
void UDP_ClientTest(std::uint16_t port);
#endif // UDP_SERVER_TEST

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		//const std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		const std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

		std::shared_ptr<dev::tDataSetConfig> DsConfig = std::make_shared<dev::tDataSetConfig>(PathFileMX);
		
		boost::asio::io_context ioc;

		tLANTagServer Server(ioc, DsConfig->GetPortUDP().Value, DsConfig);

		std::thread Thread_ioc([&]() { ioc.run(); });

		//DsConfig.reset();

#ifdef UDP_SERVER_TEST
		UDP_ClientTest(DsConfig->GetPortUDP().Value);
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
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
