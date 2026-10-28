#include <devConfig.h>

#include "dev/config.h"
#include "dev/devState.h"

//#include <utilsException.h>
#include <utilsExits.h>
#include <utilsPath.h>

#include <shareUtilsFile.h>

#include <thread>
#include <iostream> // [TEST]

#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
	dev::tConfig DsConfig = dev::tConfig(argv[0]);
	if (DsConfig.GetUART().IsWrong())
		return static_cast<int>(utils::exit_code::EX_CONFIG);
	//std::shared_ptr<dev::tConfig> DsConfig = std::make_shared<dev::tConfig>(PathFileConfig, PathFileMX);
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
