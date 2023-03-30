#include <utilsBase.h>
#include <utilsChrono.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <devDataSetConfig.h>
#include <devDataSetGNSS.h>

#include <shareConfig.h>
#include <shareHTML.h>
#include <shareUtils.h>
#include <shareUtils.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

std::deque<std::string> GetFilePaths(const share_config::tOutFile& conf)
{
	utils::RemoveFilesOutdated(conf.Path, conf.Prefix, conf.QtyMax);
	auto List = utils::GetFilesLatest(conf.Path, conf.Prefix, conf.QtyMax);
	utils::linux::CorrPaths(List);
	return List;
}

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path{ argv[0] };
		std::filesystem::path PathFile = Path.filename();
		if (PathFile.has_extension())
			PathFile.replace_extension();

		std::string PathFileConfig = utils::linux::GetPathConfigExc(PathFile.string());
		std::string PathFileDevice = utils::linux::GetPathConfigExc("mxdevice");
		std::string PathFilePrivate = utils::linux::GetPathConfigExc("mxprivate");

		utils::tTimePeriod TimePeriod(true);

		while (true)
		{
			const dev::tDataSetConfig DsConfig(PathFileConfig, PathFileDevice, PathFilePrivate);

			const share_config::tDevice ConfDevice = DsConfig.GetDevice();
			const dev::config::tEmail ConfEmail = DsConfig.GetEmail();
			const dev::config::tGNSS ConfGnss = DsConfig.GetGNSS();
			const dev::config::tPicture ConfPict = DsConfig.GetPicture();
			const dev::config::tSpyOutGLO ConfSpyOutGLO = DsConfig.GetSpyOutGLO();
			const dev::config::tSpyOutGPS ConfSpyOutGPS = DsConfig.GetSpyOutGPS();
			const dev::config::tLog ConfLog = DsConfig.GetLog();

			if (ConfEmail.IsWrong() ||
				ConfGnss.IsWrong() ||
				ConfPict.IsWrong() ||
				ConfSpyOutGLO.IsWrong() ||
				ConfSpyOutGPS.IsWrong() ||
				ConfLog.IsWrong())
				return static_cast<int>(utils::tExitCode::EX_CONFIG);
			
			if (TimePeriod.GetPeriod() != ConfEmail.Period)
				TimePeriod.Set(ConfEmail.Period, true);

			if (!TimePeriod.IsReady())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				continue;
			}

			share::tLogFile LogFile(ConfLog);
			LogFile.Write("Start");
			
			share::RemoveFilesOutdated(ConfGnss);
			share::RemoveFilesOutdated(ConfPict);
			share::RemoveFilesOutdated(ConfSpyOutGLO);
			share::RemoveFilesOutdated(ConfSpyOutGPS);

			auto GnssList = share::GetFilePaths(ConfGnss);
			auto PictList = share::GetFilePaths(ConfPict);
			auto SpyOutGLOList = share::GetFilePaths(ConfSpyOutGLO);
			auto SpyOutGPSList = share::GetFilePaths(ConfSpyOutGPS);

			LogFile.Write("Lists loaded");

			dev::tDataSetGNSS DsGNSS{};
			try
			{
				if (!GnssList.empty())
					DsGNSS = dev::tDataSetGNSS(GnssList.back());

				if (!SpyOutGLOList.empty())
					DsGNSS.SetStateGLO(SpyOutGLOList.back());

				if (!SpyOutGPSList.empty())
					DsGNSS.SetStateGPS(SpyOutGPSList.back());
			}
			catch (const std::exception& e)//JSONcat  error
			{
				std::cerr << "[ERR-JSON] " << e.what() << "\n";
			}

			LogFile.Write("DataSets loaded");

			std::string Cmd;
			Cmd = "echo ";
			Cmd += "\"<html><head><style>table{ border: 0; cellspacing: 0; cellpadding: 0; } td{ padding: 0px 10px 0px 10px; }</style></head><body>";

			Cmd += DsGNSS.GetHTMLTable("width=\"70\"");

			Cmd += share::GetHTMLTableSystemShort("width=\"80\"", ConfDevice);

			Cmd += "<body></html>\"";
			Cmd += " | mutt ";
			for (auto& i : PictList)
			{
				auto PathTemp = share::GetFilePathTemp(i);
				if (PathTemp == std::nullopt)
					continue;
				std::filesystem::rename(i, *PathTemp);
				i = PathTemp->string();

				Cmd += " -a \"";
				Cmd += i;
				Cmd += "\"";
			}
			Cmd += " -s \"" + utils::linux::CmdLine("hostname") + ": Schedule Snapshot Recording\"";
			Cmd += " -- " + ConfEmail.To;

			LogFile.Write(Cmd);

			bool ThreadSystem_ExitOK = false;
			std::thread ThreadSystem([&ThreadSystem_ExitOK, &LogFile, &Cmd]()
				{
					int ResCode = 0;
#if defined(_WIN32)
					std::cout << Cmd << '\n';
#else 
					ResCode = system(Cmd.c_str());
#endif
					LogFile.Write("ResCode: " + std::to_string(ResCode));
					ThreadSystem_ExitOK = true;
				});


			//[#] 500ms * 20 = 10000ms => 10s waits for sending email
			for (int i = 0; i < 20; ++i)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				if (ThreadSystem_ExitOK)
				{
					ThreadSystem.join();
					break;
				}
			}

			if (!ThreadSystem_ExitOK)
			{
				std::cerr << "[ERR] \"system\" failed\n";
				// The program is being closed without joining existing thread.
				exit(static_cast<int>(utils::tExitCode::EX_IOERR));
				// If "return" is called while the program is being terminated without joining existing thread.
				// The following message appears:
				// "terminate called without an active exception
				// Aborted(core dumped)"
				//return static_cast<int>(utils::tExitCode::EX_IOERR);
			}

			// Do not try to remove them in case of trouble with the "system" thread.
			// They will be removed at next start of this application.
			for (auto& i : PictList)
				std::filesystem::remove(i);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";
		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
