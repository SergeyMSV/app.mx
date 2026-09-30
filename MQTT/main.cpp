#include "main.h"

#include "dev/connection.h"

#include <future>
#include <thread>

#include <utilsExits.h>
#include <utilsPath.h>
#include <shareLog.h>
#include <utilsTime.h>

#include "dev/config.h"

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		//const std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		const std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

		//dev::tDataSetConfig DsConfig = dev::tDataSetConfig(PathFileMX);
		std::shared_ptr<dev::tDataSetConfig> DsConfig = std::make_shared<dev::tDataSetConfig>(PathFileMX);

		while (true)
		{
			try
			{
				share::tMeasureDuration Measure("Sending measurements...");

				const std::string SensorData = utils::time::tDateTime::Now().ToString();

				try
				{
					std::future<void> TaskConnectionFuture = std::async(std::launch::async, dev::TaskConnectionHandler, "test.mosquitto.org", "1883", DsConfig);
					//std::future<void> TaskConnectionFuture = std::async(std::launch::async, TaskConnectionHandler, "test.mosquitto.org", "1883", SensorData);
					//std::future<void> TaskConnectionFuture = std::async(std::launch::deferred, TaskConnectHandler, std::ref(Socket)); // a task is not started by wait_for(..), it'll be deferred forever

					TaskConnectionFuture.get();
				}
				catch (std::exception& ex)
				{
					g_Log.Exception(ex.what());
				}
			}
			catch (std::exception& ex)
			{
				g_Log.Exception(ex.what());
			}

			g_Log.TestMessage("NO CONNECTION");

			share::tMeasureDuration Measure("Sleeping...");
			std::this_thread::sleep_for(std::chrono::seconds(60)); // [#] pause - it can be in the settings
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::exit_code::EX_IOERR);
	}
	return utils::exit_code::EX_OK;
}