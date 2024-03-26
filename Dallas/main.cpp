#include "main.h"
#include "main_server.h"

#include <devDataSetConfig.h>

#include <utilsBase.h>
#include <utilsPath.h>

#include <array>
#include <future>
#include <iostream>
#include <memory>
#include <string>

#ifdef UDP_SERVER_TEST
void UDP_ClientTest(std::uint16_t port);
#endif // UDP_SERVER_TEST

void ThreadDallas(std::promise<void>& promise, const std::shared_ptr<dev::tDataSetConfig>& config, tUDPServer& server, const std::shared_ptr<tQueuePack>& queueIn);

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		//const std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		const std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

		std::shared_ptr<dev::tDataSetConfig> DsConfig = std::make_shared<dev::tDataSetConfig>(PathFileMX);
		if (DsConfig->GetUDPPort().IsWrong())
			THROW_RUNTIME_ERROR("IP-port is not valid");

		std::shared_ptr<tQueuePack> QueueIn = std::make_shared<tQueuePack>();

		boost::asio::io_context ioc;
		tUDPServer Server(ioc, DsConfig->GetUDPPort().Value, QueueIn);
		std::thread Th_ioc([&]() { ioc.run(); });
		
		std::promise<void> ThDls_Promise;
		auto ThDls_Future = ThDls_Promise.get_future();
		std::thread ThDls(ThreadDallas, std::ref(ThDls_Promise), std::ref(DsConfig), std::ref(Server), std::ref(QueueIn));

#ifdef UDP_SERVER_TEST
		std::thread ThClientTest([&DsConfig]() { UDP_ClientTest(DsConfig->GetUDPPort().Value); });
		ThClientTest.detach();
#endif // UDP_SERVER_TEST

		try
		{
			ThDls_Future.wait();
			std::cerr << "\nCLOSED\n";
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}

		// [TBD] all the threads is to be informed that the application ends

		DsConfig.reset(); // All threads will notice it and stop. In other words it stops all the threads.

		ioc.stop();
		Th_ioc.join();
		ThDls.join();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::tExitCode::EX_IOERR);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
