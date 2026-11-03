#include <devConfig.h>

#include "dev/config.h"
#include "dev/state.h"

#include <utilsExits.h>

#include <shareUtilsFile.h>

#include <thread>
#include <iostream>

#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
	dev::tConfig DsConfig = dev::tConfig(argv[0]);
	share::RemoveFilesOutdated(DsConfig.GetOutFile());
	if (DsConfig.GetUART().IsWrong())
		return static_cast<int>(utils::exit_code::EX_CONFIG);

	int ExitCode = utils::exit_code::EX_OK;

	boost::asio::io_context ioc;
	std::thread Thread_ioc;

	try
	{
		share::tLogFileLine LogFile(DsConfig.GetLog());
		LogFile.WriteLine("Start");

		const share::config::port::tUART_Config UARTConfig = DsConfig.GetUART();
		dev::tPortUART Port(ioc, UARTConfig.ID, UARTConfig.BR);

		Thread_ioc = std::thread([&ioc]() { ioc.run(); });

		auto [DsHW, Status] = dev::state::Init(Port, DsConfig);

		LogFile.WriteLine(DsHW.ToString());

		if (Status == dev::state::tStatus::None)
			Status = dev::state::Receive(Port, DsConfig, DsHW);


		if (Status != dev::state::tStatus::None)
		{
			std::cerr << dev::state::ToString(Status) << '\n';
			ExitCode = utils::exit_code::EX_IOERR;
		}
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
