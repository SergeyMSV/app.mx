#include <devConfig.h>

#include <devCamera.h>
#include <devLog.h>
#include <devDataSetConfig.h>
#include <devShell.h>

#include <utilsBase.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

#include <boost/asio.hpp>

namespace dev
{
	void ThreadFunShell();
}

tDataSetMainControl g_DataSetMainControl;

void Thread_CAM_Handler(std::promise<bool>& promise)
{
	dev::tLog Log(dev::tLog::tID::CAM, "CAM");

	Log.LogSettings.Field.CAM = 1;

	boost::asio::io_context IO;

	try
	{
		dev::tCamera Dev(&Log, IO);

		std::thread Thread_IO([&]() { IO.run(); });

		bool Thread_Dev_Exists = true;
		bool Thread_Dev_ExistOnError = false;
		std::thread Thread_Dev([&]()
			{
				try
				{
					//if (g_DataSetMainControl.Thread_CAM_State == tDataSetMainControl::tStateCAM::Exit)//[TBD] TEST
					//	Thread_Dev_Exists = false;
					Dev();
					Thread_Dev_Exists = false;
					const std::string ErrMsg = Dev.GetLastErrorMsg();
					if (!ErrMsg.empty())
					{
						std::cerr << ErrMsg << "\n";
						Thread_Dev_ExistOnError = true;
					}
				}
				catch (...)
				{
					Thread_Dev_Exists = false;
					promise.set_exception(std::current_exception());
				}
			});

		tDataSetMainControl::tStateCAM StateGNSSPrev = g_DataSetMainControl.Thread_CAM_State;

		while (true)
		{
			if (!Thread_Dev_Exists)
				break;

			if (g_DataSetMainControl.Thread_CAM_State != tDataSetMainControl::tStateCAM::Nothing)
			{
				switch (g_DataSetMainControl.Thread_CAM_State)
				{
				case tDataSetMainControl::tStateCAM::Start: Dev.Start(); break;
				case tDataSetMainControl::tStateCAM::Halt: Dev.Halt(); break;
				//case tDataSetMainControl::tStateCAM::Restart: Dev.Restart(); break;
				case tDataSetMainControl::tStateCAM::Exit: Dev.Exit(); break;
				}

				if (g_DataSetMainControl.Thread_CAM_State == tDataSetMainControl::tStateCAM::Exit)
					break;

				g_DataSetMainControl.Thread_CAM_State = tDataSetMainControl::tStateCAM::Nothing;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		Thread_Dev.join();

		IO.stop();

		Thread_IO.join();

		if (!Thread_Dev_Exists)
			promise.set_value(Thread_Dev_ExistOnError);
	}
	catch (...)
	{
		promise.set_exception(std::current_exception());
	}
}

int main(int argc, char* argv[])
{
	const bool ShellEnabled = argc >= 2 && !strcmp(argv[1], "shell");

	dev::tLog::LogSettings.Value = 0;
	dev::tLog::LogSettings.Field.Enabled = ShellEnabled ? 1 : 0;

	try
	{
		const std::filesystem::path Path{ argv[0] };
		std::filesystem::path PathFile = Path.filename();
		if (PathFile.has_extension())
			PathFile.replace_extension();

		std::string FileNameConf = utils::linux::GetPathConfig(PathFile.string());
		if (FileNameConf.empty())
			throw std::runtime_error{ "config file is not found" };

		dev::g_Settings = dev::tDataSetConfig(FileNameConf);
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_CONFIG);
	}

	utils::tExitCode CErr = utils::tExitCode::EX_OK;
	////////////////////////////////
	std::thread Thread_Shell;

	if (ShellEnabled)
		Thread_Shell = std::thread(dev::ThreadFunShell);
	////////////////////////////////

	dev::config::tOutPicture OutPicture = dev::g_Settings.GetOutPicture();
	utils::RemoveFilesOutdated(OutPicture.Path, OutPicture.Prefix, OutPicture.QtyMax);
	utils::RemoveFilesOutdated(OutPicture.Path, g_FileNameTempPrefix + OutPicture.Prefix, 0);

	////////////////////////////////

	std::promise<bool> Thread_CAM_Promise;
	auto Thread_CAM_Future = Thread_CAM_Promise.get_future();

	std::thread Thread_CAM(Thread_CAM_Handler, std::ref(Thread_CAM_Promise));//C++11

	try
	{
		if (Thread_CAM_Future.get())
			CErr = utils::tExitCode::EX_NOINPUT;
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";

		g_DataSetMainControl.Thread_CAM_State = tDataSetMainControl::tStateCAM::Exit;

		CErr = utils::tExitCode::EX_IOERR;
	}

	Thread_CAM.join();

	if (ShellEnabled)
		Thread_Shell.detach();

	return static_cast<int>(CErr);
}
