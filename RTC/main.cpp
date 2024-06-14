#include "main.h"
//#include "main_server.h"

#include <devDataSet.h>
#include <devDataSetConfig.h>

#include <sharePipe.h>

//#include <utilsBase.h>
#include <utilsPath.h>

//#include <array>
#include <future>
#include <iostream>
#include <memory>
//#include <string>

//#include <cstdlib>

//#ifdef UDP_SERVER_TEST
//void UDP_ClientTest(std::uint16_t port);
//#endif // UDP_SERVER_TEST

void ThreadDallas(std::promise<void>& promise, const std::shared_ptr<dev::tDataSetConfig>& config, const std::shared_ptr<dev::tDataSet>& dataSet);// , tUDPServer& server, const std::shared_ptr<tQueuePack>& queueIn);

int main(int argc, char* argv[])
{
	utils::tExitCode ExitCode = utils::tExitCode::EX_OK;

	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		const std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();
		const std::string PathFileMXDev = utils::path::GetPathConfigExc("mxdev").string();

		std::shared_ptr<dev::tDataSetConfig> DsConfig = std::make_shared<dev::tDataSetConfig>(PathFileMX, PathFileMXDev);
		std::shared_ptr<dev::tDataSet> DataSet = std::make_shared<dev::tDataSet>();

		//std::shared_ptr<share::tPipeO<std::string>> PipeO = std::make_shared<share::tPipeO<std::string>>(DsConfig->GetPipe().Path, [&]() { return DataSet->GetDemoStr(); });



		////if (DsConfig->GetUDPPort().IsWrong())
		////	THROW_RUNTIME_ERROR("IP-port is not valid");

		////std::shared_ptr<tQueuePack> QueueIn = std::make_shared<tQueuePack>();

		////boost::asio::io_context ioc;
		////tUDPServer Server(ioc, DsConfig->GetUDPPort().Value, QueueIn);
		////std::thread Th_ioc([&]() { ioc.run(); });
		
		std::promise<void> ThDls_Promise;
		auto ThDls_Future = ThDls_Promise.get_future();
		std::thread ThDls(ThreadDallas, std::ref(ThDls_Promise), std::ref(DsConfig), std::ref(DataSet));// , std::ref(Server), std::ref(QueueIn));

		try
		{
			ThDls_Future.get(); // in case of wait() it doesn't transition to "catch" block
			std::cerr << "\nCLOSED\n";
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << '\n';
			ExitCode = utils::tExitCode::EX_IOERR;
		}

		// [TBD] all the threads is to be informed that the application ends

		DsConfig.reset(); // All threads will notice it and stop. In other words it stops all the threads.

		//ioc.stop();
		//Th_ioc.join();
		ThDls.join();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		if (ExitCode == utils::tExitCode::EX_OK)
			ExitCode = utils::tExitCode::EX_SOFTWARE;
	}

	return static_cast<int>(ExitCode);
}
