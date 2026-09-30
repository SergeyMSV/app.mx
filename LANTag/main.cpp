#include "main.h"
#include "dev/config.h"
#include "dev/server.h"

#include <utilsExits.h>
#include <utilsPath.h>

#include <iostream>
#include <memory>
#include <string>

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		//const std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		const std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

		std::shared_ptr<dev::tConfig> DsConfig = std::make_shared<dev::tConfig>(PathFileMX);
		
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
