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

//$ echo "Hello this is the body message, we are sending email with attachement using mutt and msmtp" | mutt -a "/home/sergey/cam.jpg" -a "/home/sergey/cam_2.jpg" -s "this is the subject of the message" --satellite4@yandex.ru
//$ cat ~/email/attached_file.txt | mutt -a "/home/sergey/cam.jpg" -a "/home/sergey/cam_2.jpg" -s "this is the subject of the message" --satellite4@yandex.ru

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
				Cmd += " -a \"";
				Cmd += i;
				Cmd += "\"";
			}
			Cmd += " -s \"" + utils::linux::CmdLine("hostname") + ": Schedule Snapshot Recording\"";
			Cmd += " -- " + ConfEmail.To;

			LogFile.Write(Cmd);

			int ResCode = 0;
#if defined(_WIN32)
			std::cout << Cmd << '\n';
#else 
			ResCode = system(Cmd.c_str());
#endif
			LogFile.Write("ResCode: " + std::to_string(ResCode));
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
