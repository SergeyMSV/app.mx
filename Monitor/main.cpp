#include <utilsBase.h>
#include <utilsChrono.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <shareConfig.h>
#include <shareHTML.h>

#include <devDataSetConfig.h>
#include <devMonitorMemory.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <thread>

//$ echo "Hello this is the body message, we are sending email with attachement using mutt and msmtp" | mutt -a "/home/sergey/cam.jpg" -a "/home/sergey/cam_2.jpg" -s "this is the subject of the message" --satellite4@yandex.ru
//$ cat ~/email/attached_file.txt | mutt -a "/home/sergey/cam.jpg" -a "/home/sergey/cam_2.jpg" -s "this is the subject of the message" --satellite4@yandex.ru

void SendEMail(const dev::tDataSetConfig& config, const dev::tMonitorMemory& mem)
{
	std::string Cmd;
	Cmd = "echo ";
	Cmd += "\"<html><head><style>table{ border: 0; cellspacing: 0; cellpadding: 0; } td{ padding: 0px 10px 0px 10px; }</style></head><body>";

	Cmd += mem.GetHTMLTable("width=\"80\"");
	Cmd += share::GetHTMLTableSystem("width=\"80\"", config.GetDevice());
	Cmd += "<body></html>\"";
	Cmd += " | mutt ";
	Cmd += " -s \"" + utils::linux::CmdLine("hostname") + ": Schedule Snapshot Recording\"";
	Cmd += " -- " + config.GetEmail().To;

	int ResCode = 0;

#if defined(_WIN32)
	std::cout << Cmd << '\n';
#else 
	ResCode = system(Cmd.c_str());
#endif
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

		dev::tMonitorMemory MonitorMemory;

		utils::tTimePeriod TimePeriod(false);
		utils::tTimePeriod TimePeriodMem(true);

		while (true)
		{
			const dev::tDataSetConfig DsConfig(PathFileConfig, PathFileDevice, PathFilePrivate);

			if (DsConfig.GetEmail().IsWrong())
				return static_cast<int>(utils::tExitCode::EX_CONFIG);

			TimePeriod.Set(DsConfig.GetEmail().PeriodMinimum);
			TimePeriodMem.Set(DsConfig.GetBoardRAM().Period);

			if (TimePeriodMem.IsReady())
				MonitorMemory.Control(DsConfig);

			//if (TimePeriod_SmthElse.IsReady())
			//	Monitor_SmthElse.Control(DsConfig);

			// One more Control ...

			if (MonitorMemory.IsEvent() && TimePeriod.IsReady())
			{
				SendEMail(DsConfig, MonitorMemory);
				MonitorMemory.ResetEvent();
			}			

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
