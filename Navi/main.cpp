#include <utilsBase.h>
#include <utilsChrono.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <devDataSetConfig.h>
#include <devDataSetGNSS.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <thread>

//$ echo "Hello this is the body message, we are sending email with attachement using mutt and msmtp" | mutt -a "/home/sergey/cam.jpg" -a "/home/sergey/cam_2.jpg" -s "this is the subject of the message" --satellite4@yandex.ru
//$ cat ~/email/attached_file.txt | mutt -a "/home/sergey/cam.jpg" -a "/home/sergey/cam_2.jpg" -s "this is the subject of the message" --satellite4@yandex.ru

std::string GetHTMLTableDevice(const std::string& styleCol1)
{
	std::string Table;
	Table += "<table>";
	Table += "<tr><td " + styleCol1 + "><b>DEVICE</b></td><td>" + utils::linux::CmdLine("hostname") + "</td></tr>";
	Table += "<tr><td>IP Address</td><td>" + utils::linux::CmdLine("ip address | grep 'inet ' | grep -v 127.0.0.1 | cut -d\\t -f2 | cut -d/ -f1 | awk '{print $1}'") + "</td></tr>";

	std::string Uptime = utils::linux::GetUptimeString();
	Table += "<tr><td>Uptime</td><td>" + Uptime + "</td></tr>";
	Table += "<tr><td>Load average</td><td>" + utils::linux::CmdLine("cat /proc/loadavg") + "</td></tr>";
	Table += "<tr><td>Date</td><td>" + utils::linux::CmdLine("date") + "</td></tr>";

	Table += "</table>";

	return Table;
}

std::string GetHTMLTableSystem(const std::string& styleCol1, const dev::config::tDevice& dsConfigDevice)
{
	std::stringstream Table;
	Table << "<table border=\"1\">";
	Table << "<tr><td " << styleCol1 << "><b>SYSTEM</b></td></tr>";
	Table << "<tr><td>Model</td><td>" << dsConfigDevice.Type << "</td></tr>";
	Table << "<tr><td>Version</td><td>" << dsConfigDevice.Version.ToString() << "</td></tr>";
	utils::linux::tCpuInfo CpuInfo = utils::linux::GetCpuInfo();
	Table << "<tr><td>CPU model</td><td>" << CpuInfo.ModelName << "</td></tr>";
	Table << std::fixed;
	Table.precision(2);
	Table << "<tr><td>BogoMIPS</td><td>" << CpuInfo.BogoMIPS << "</td></tr>";
	Table << "<tr><td>Hardware</td><td>" << CpuInfo.Hardware << "</td></tr>";
	Table << "<tr><td>Linux</td><td>" << utils::linux::CmdLine("uname -a") << "</td></tr>";
	Table << "<tr><td>Linux build</td><td>" << utils::linux::CmdLine("cat /proc/version") << "</td></tr>";
	Table << "<tr><td>HW clock</td><td>" << utils::linux::CmdLine("hwclock -r") << "</td></tr>";
	Table << "</table>";
	return Table.str();
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

		utils::tTimePeriod TimePeriod;

		while (true)
		{
			const dev::tDataSetConfig DsConfig(PathFileConfig, PathFileDevice, PathFilePrivate);

			const dev::config::tDevice ConfDevice = DsConfig.GetDevice();
			const dev::config::tEmail ConfEmail = DsConfig.GetEmail();
			const dev::config::tGNSS ConfGnss = DsConfig.GetGNSS();
			const dev::config::tPicture ConfPicture = DsConfig.GetPicture();

			if (ConfEmail.IsWrong() || ConfGnss.IsWrong() || ConfPicture.IsWrong())
				return static_cast<int>(utils::tExitCode::EX_CONFIG);
			
			TimePeriod.Set(ConfEmail.Period);

			if (!TimePeriod.IsReady())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			utils::RemoveFilesOutdated(ConfGnss.Path, ConfGnss.Prefix, ConfGnss.QtyMax);
			auto GnssList = utils::GetFilesLatest(ConfGnss.Path, ConfGnss.Prefix, ConfGnss.QtyMax);
			utils::linux::CorrPaths(GnssList);

			utils::RemoveFilesOutdated(ConfPicture.Path, ConfPicture.Prefix, ConfPicture.QtyMax);
			auto PictureList = utils::GetFilesLatest(ConfPicture.Path, ConfPicture.Prefix, ConfPicture.QtyMax);
			utils::linux::CorrPaths(PictureList);

			dev::tDataSetGNSS DsGNSS{};
			try
			{
				if (!GnssList.empty())
					DsGNSS = dev::tDataSetGNSS(GnssList.back());
			}
			catch (...) {}//JSON error

			std::string Cmd;
			Cmd = "echo ";
			Cmd += "\"<html><head><style>table{ border: 0; cellspacing: 0; cellpadding: 0; } td{ padding: 0px 20px 0px 0px; }</style></head><body>";

			Cmd += "<table width=\"400\">";
			Cmd += "<tr><td>";
			Cmd += GetHTMLTableDevice("width=\"100\"");
			Cmd += "</td></tr>";

			Cmd += "<tr><td>";
			Cmd += DsGNSS.GetHTMLTable("width=\"100\"");
			Cmd += "</td></tr>";

			Cmd += "<tr><td>";
			Cmd += GetHTMLTableSystem("width=\"100\"", ConfDevice);
			Cmd += "</td></tr>";
			Cmd += "</table>";
			Cmd += "<body></html>\"";
			Cmd += " | mutt ";
			for (auto& i : PictureList)
			{
				Cmd += " -a \"";
				Cmd += i;
				Cmd += "\"";
			}
			Cmd += " -s \"" + utils::linux::CmdLine("hostname") + ": Schedule Snapshot Recording\"";
			Cmd += " -- " + ConfEmail.To;

			int ResCode = 0;

#if defined(_WIN32)
			std::cout << Cmd << '\n';
#else 
			ResCode = system(Cmd.c_str());
#endif

			if (ResCode == 0)//[TBD] check it
			{
				for (auto& i : PictureList)
				{
					std::error_code ErrCode;
					std::filesystem::remove(i, ErrCode);
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
