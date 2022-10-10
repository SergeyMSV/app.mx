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

std::string GetHTMLTableSystem(const std::string& styleCol1)
{
	std::stringstream Table;
	Table << "<table>";
	Table << "<tr><td " << styleCol1 << "><b>SYSTEM</b></td></tr>";
	utils::linux::tCpuInfo CpuInfo = utils::linux::GetCpuInfo();
	Table << "<tr><td>CPU model</td><td>" << CpuInfo.ModelName << "</td></tr>";
	Table << std::fixed;
	Table.precision(2);
	Table << "<tr><td>BogoMIPS</td><td>" << CpuInfo.BogoMIPS << "</td></tr>";
	Table << "<tr><td>Hardware</td><td>" << CpuInfo.Hardware << "</td></tr>";
	Table << "<tr><td>Firmware</td><td>" << utils::linux::CmdLine("uname -a") << "</td></tr>";
	Table << "<tr><td>Linux</td><td>" << utils::linux::CmdLine("cat /proc/version") << "</td></tr>";
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

		std::string FileNameConf = utils::linux::GetPathConfig(PathFile.string());

		utils::tTimePeriod TimePeriod;

		while (true)
		{
			const dev::tDataSetConfig DsConfig(FileNameConf);

			const dev::config::tEmail Email = DsConfig.GetEmail();
			const dev::config::tGNSS Gnss = DsConfig.GetGNSS();
			const dev::config::tPicture Picture = DsConfig.GetPicture();

			if (Email.IsWrong() || Gnss.IsWrong() || Picture.IsWrong())
				return static_cast<int>(utils::tExitCode::EX_CONFIG);
			
			TimePeriod.Set(Email.Period);

			if (!TimePeriod.IsReady())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				continue;
			}

			utils::RemoveFilesOutdated(Gnss.Path, Gnss.Prefix, Gnss.QtyMax);
			auto GnssList = utils::GetFilesLatest(Gnss.Path, Gnss.Prefix, Gnss.QtyMax);

			utils::RemoveFilesOutdated(Picture.Path, Picture.Prefix, Picture.QtyMax);
			auto PictureList = utils::GetFilesLatest(Picture.Path, Picture.Prefix, Picture.QtyMax);

			dev::tDataSetGNSS DsGNSS{};
			try
			{
				if (!GnssList.empty())
					DsGNSS = dev::tDataSetGNSS(utils::linux::GetPath(GnssList.back()));
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
			Cmd += GetHTMLTableSystem("width=\"100\"");
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
			Cmd += " -- " + Email.To;

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
