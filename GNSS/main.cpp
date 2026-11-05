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
	std::thread iocThread;

	share::tLogFileLine LogFile(DsConfig.GetLog());
	LogFile.WriteLine("Start");

	while (true)
	{
		try
		{
			const share::config::port::tUART_Config UARTConfig = DsConfig.GetUART();
			dev::tPortUART Port(ioc, UARTConfig.ID, UARTConfig.BR);

			iocThread = std::thread([&ioc]() { ioc.run(); });

			auto [DsHW, Status] = dev::state::Init(Port, DsConfig);
			if (Status != dev::state::tStatus::None)
			{
				LogFile.WriteLine(dev::state::ToString(Status));
				std::this_thread::sleep_for(std::chrono::seconds(10)); // [#]
				continue;
			}
			LogFile.WriteLine(DsHW.ToString());
			Status = dev::state::Receive(Port, DsConfig, DsHW, LogFile);
			if (Status != dev::state::tStatus::None)
			{
				LogFile.WriteLine(dev::state::ToString(Status));
				std::this_thread::sleep_for(std::chrono::seconds(10)); // [#]
				continue;
			}
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << '\n';
			ExitCode = utils::exit_code::EX_IOERR;
			break;
		}
	}

	ioc.stop();
	if (iocThread.joinable())
		iocThread.join();

	return ExitCode;
}
