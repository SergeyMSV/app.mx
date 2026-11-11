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

	const share::config::tOutFileCap logConfig = config.GetLog();
	tLog log(logConfig.Path, logConfig.QtyMax, logConfig.Prefix, logConfig.Capacity);
	log.WriteLine("Start");

	boost::asio::io_context ioc;
	auto work_guard = boost::asio::make_work_guard(ioc);
	std::thread iocThread = std::thread([&ioc, &log]() {
		try
		{
			ioc.run();
		}
		catch (const std::exception& e)
		{
			log.WriteLine(std::string("ioc thread: ") + e.what());
		}
		});

	const share::config::port::tUART_Config UARTConfig = config.GetUART();

	bool restart = false;
	while (true)
	{
		if (restart)
		{
			std::this_thread::sleep_for(std::chrono::seconds(10)); // [#]
		}
		else
		{
			restart = true;
		}

		try
		{
			dev::tPortUART Port(ioc, UARTConfig.ID, UARTConfig.BR);

			auto [DsHW, Status] = dev::state::Init(Port, config);
			if (Status != dev::state::tStatus::None)
			{
				log.WriteLine(dev::state::ToString(Status));
				continue;
			}
			log.WriteLine(DsHW.ToString());
			Status = dev::state::Receive(Port, config, DsHW, log);
			if (Status != dev::state::tStatus::None)
			{
				log.WriteLine(dev::state::ToString(Status));
				continue;
			}
		}
		catch (std::exception& e)
		{
			log.WriteLine(e.what());
			std::cerr << e.what() << '\n';
			break;
		}
	}
	work_guard.reset();
	ioc.stop();
	if (iocThread.joinable())
		iocThread.join();
	return utils::exit_code::EX_OK;
}
