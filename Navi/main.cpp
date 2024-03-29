#include <devDataSetConfig.h>
#include <devDataSetGNSS.h>

#include <utilsBase.h>
#include <utilsChrono.h>
#include <utilsFile.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <shareConfig.h>
#include <shareHTML.h>
#include <shareUtilsFile.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

//$ echo "Hello this is the body message, we are sending email with attachement using mutt and msmtp" | mutt -a "/home/sergey/cam.jpg" -a "/home/sergey/cam_2.jpg" -s "this is the subject of the message" --satellite4@yandex.ru
//$ cat ~/email/attached_file.txt | mutt -a "/home/sergey/cam.jpg" -a "/home/sergey/cam_2.jpg" -s "this is the subject of the message" --satellite4@yandex.ru

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		std::string PathFileDevice = utils::path::GetPathConfigExc("mxdevice").string();
		std::string PathFilePrivate = utils::path::GetPathConfigExc("mxprivate").string();

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
			const dev::config::tOutPicture OutPicture = DsConfig.GetOutPicture();
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

			share::tLogFileLine LogFile(ConfLog);
			LogFile.Write("Start");
			
			utils::file::RemoveFilesWithPrefix(OutPicture.Path, OutPicture.Prefix); // Remove all possible abandoned files.

			auto GnssList = share::GetFilePaths(ConfGnss);

			std::vector<std::string> PictListTemp;
			{
				auto PictList = share::GetFilePaths(ConfPict);

				for (auto i = PictList.crbegin(); i != PictList.crend(); ++i)
				{
					auto FileNameNew = utils::file::ReplaceFileNamePrefix(*i, ConfPict.Prefix, OutPicture.Prefix);
					if (!FileNameNew.has_value())
						continue;

					if (std::filesystem::copy_file(*i, *FileNameNew))
						PictListTemp.push_back(*FileNameNew);

					if (OutPicture.QtyMax == PictListTemp.size())
						break;
				}
			}

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
				std::cerr << utils::GetLogMessage(e.what(), __FILE__, __LINE__) << '\n';
			}

			LogFile.Write("DataSets loaded");

			std::string Cmd;
			Cmd = "echo ";
			Cmd += "\"<html><head><style>table{ border: 0; cellspacing: 0; cellpadding: 0; } td{ padding: 0px 10px 0px 10px; }</style></head><body>";

			Cmd += DsGNSS.GetHTMLTable("width=\"70\"");

			Cmd += share::GetHTMLTableSystemShort("width=\"80\"", ConfDevice);

			Cmd += "<body></html>\"";
			Cmd += " | mutt ";
			for (auto& i : PictListTemp)
			{
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


			//[#] 500ms * 120 = 60000ms => 60s it waits for sending email
			for (int i = 0; i < 120; ++i)
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
				std::cerr << utils::GetLogMessage("\"system\" failed", __FILE__, __LINE__) << '\n';
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
			for (auto& i : PictListTemp)
				std::filesystem::remove(i);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
