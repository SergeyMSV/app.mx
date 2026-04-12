#include <devConfig.h>

#include <devDataSetConfig.h>
#include <devState.h>

//#include <utilsException.h>
#include <utilsExits.h>
#include <utilsPath.h>

#include <shareUtilsFile.h>

#include <thread>
#include <iostream> // [TEST]

#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
	const std::filesystem::path Path(argv[0]);
	const std::string AppName = utils::path::GetAppNameMain(Path);
	const std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
	const std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

	dev::tDataSetConfig DsConfig = dev::tDataSetConfig(PathFileConfig, PathFileMX);
	if (DsConfig.GetUART().IsWrong())
		return static_cast<int>(utils::exit_code::EX_CONFIG);
	//std::shared_ptr<dev::tDataSetConfig> DsConfig = std::make_shared<dev::tDataSetConfig>(PathFileConfig, PathFileMX);
	//if (DsConfig->GetUART().IsWrong())
	//	return static_cast<int>(utils::exit_code::EX_CONFIG);
		//THROW_RUNTIME_ERROR("UART is not valid.");

	int ExitCode = utils::exit_code::EX_OK;

	boost::asio::io_context ioc;
	std::thread Thread_ioc;

	try
	{
		share::tLogFileLine LogFile(DsConfig.GetLog());
		LogFile.Write("Start");

		share::RemoveFilesOutdated(DsConfig.GetOutGNSS());
		share::RemoveFilesOutdated(DsConfig.GetOutGNSS(), dev::settings::FileNameTempPrefix);

		const share::config::port::tUART_Config UARTConfig = DsConfig.GetUART();
		dev::tPortUART Port(ioc, UARTConfig.ID, UARTConfig.BR);

		Thread_ioc = std::thread([&ioc]() { ioc.run(); });

		auto [DsHW, Status] = dev::state::Init(Port, DsConfig);

		std::cout << DsHW.ToString() << '\n'; // [TBD] write into log

		if (Status != dev::state::tStatus::None)
		{
			// [TBD] Handle the error!
		}

		Status = dev::state::Receive(Port, DsConfig, DsHW);
		if (Status != dev::state::tStatus::None)
		{
			// [TBD] Handle the error!
		}

		// [TBD] Error Handling or exit

	}
	//catch(...){}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		ExitCode = utils::exit_code::EX_IOERR; // [TBD] check it
	}

	ioc.stop();
	if (Thread_ioc.joinable())
		Thread_ioc.join();

	return ExitCode;
}
