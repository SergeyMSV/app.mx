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
	dev::tConfig config = dev::tConfig(argv[0]);
	share::RemoveFilesOutdated(config.GetOutFile());
	if (config.GetUART().IsWrong())
		return utils::exit_code::EX_CONFIG;

	boost::asio::io_context ioc;
	std::thread iocThread;

	const share::config::tOutFileCap logConfig = config.GetLog();
	tLog log(logConfig.Path, logConfig.QtyMax, logConfig.Prefix, logConfig.Capacity);
	log.WriteLine("Start");

	const auto StopIO = [&ioc, &iocThread]()
		{
			ioc.stop();
			if (iocThread.joinable())
				iocThread.join();
		};

	const auto HandleError = [&log, &StopIO](dev::state::tStatus status)
		{
			log.WriteLine(dev::state::ToString(status));
			StopIO();
			std::this_thread::sleep_for(std::chrono::seconds(10)); // [#]
		};

	while (true)
	{
		try
		{
			const share::config::port::tUART_Config UARTConfig = config.GetUART();
			dev::tPortUART Port(ioc, UARTConfig.ID, UARTConfig.BR);

			ioc.restart(); // It's necessary after ioc.stop().
			iocThread = std::thread([&ioc, &log]() {
				try
				{
					ioc.run();
				}
				catch (const std::exception& e)
				{
					log.WriteLine(std::string("ioc thread: ") + e.what());
				}
				});

			auto [DsHW, Status] = dev::state::Init(Port, config);
			if (Status != dev::state::tStatus::None)
			{
				HandleError(Status);
				continue;
			}
			log.WriteLine(DsHW.ToString());
			Status = dev::state::Receive(Port, config, DsHW, log);
			if (Status != dev::state::tStatus::None)
			{
				HandleError(Status);
				continue;
			}
		}
		catch (std::exception& e)
		{
			log.WriteLine(e.what());
			std::cerr << e.what() << '\n';
			break;
		}

		StopIO();
	}

	//StopIO();

	return utils::exit_code::EX_OK;
}
