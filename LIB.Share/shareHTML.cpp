#include "shareConfig.h"
#include "shareHTML.h"

#include <utilsLinux.h>

#include <sstream>
#include <string>

namespace share
{

std::string GetHTMLBgColour(tHTMLFieldStatus status)
{
	switch (status)
	{
	case tHTMLFieldStatus::Norm: return " bgcolor=\"lightblue\"";
	case tHTMLFieldStatus::NormAgain: return " bgcolor=\"lightgreen\"";
	case tHTMLFieldStatus::Warn: return " bgcolor=\"pink\"";
	case tHTMLFieldStatus::Super: return " bgcolor=\"cyan\"";
	case tHTMLFieldStatus::TableHeader:return " bgcolor=\"lightgray\"";
	default: break;
	}

	return {};
}

std::string GetHTMLTableDevice(const std::string& styleCol1)
{
	std::string StyleHeader = share::GetHTMLBgColour(share::tHTMLFieldStatus::TableHeader);

	std::stringstream Table;
	Table << "<table>";
	Table << "<tr><td " << styleCol1 << "><b>DEVICE</b></td><td>" << utils::linux::CmdLine("hostname") << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">IP Address</td><td>" << utils::linux::CmdLine("ip address | grep 'inet ' | grep -v 127.0.0.1 | cut -d\\t -f2 | cut -d/ -f1 | awk '{print $1}'") << "</td></tr>";

	std::string Uptime = utils::linux::GetUptimeString();
	Table << "<tr><td" << StyleHeader << ">Uptime</td><td>" + Uptime << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Load average</td><td>" << utils::linux::CmdLine("cat /proc/loadavg") << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Date</td><td>" << utils::linux::CmdLine("date") << "</td></tr>";

	Table << "</table>";

	return Table.str();
}

std::string GetHTMLTableSystem(const std::string& styleCol1, const share_config::tDevice& dsConfigDevice)
{
	std::string StyleHeader = share::GetHTMLBgColour(share::tHTMLFieldStatus::TableHeader);

	std::stringstream Table;
	Table << "<table>";
	Table << "<tr><td " << styleCol1 << "><b>SYSTEM</b></td></tr>";
	Table << "<tr><td" << StyleHeader << ">Model</td><td>" << dsConfigDevice.Type << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Version</td><td>" << dsConfigDevice.Version.ToString() << "</td></tr>";
	utils::linux::tCpuInfo CpuInfo = utils::linux::GetCpuInfo();
	Table << "<tr><td" << StyleHeader << ">CPU model</td><td>" << CpuInfo.ModelName << "</td></tr>";
	Table << std::fixed;
	Table.precision(2);
	Table << "<tr><td" << StyleHeader << ">BogoMIPS</td><td>" << CpuInfo.BogoMIPS << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Hardware</td><td>" << CpuInfo.Hardware << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Linux</td><td>" << utils::linux::CmdLine("uname -a") << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Linux build</td><td>" << utils::linux::CmdLine("cat /proc/version") << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">HW clock</td><td>" << utils::linux::CmdLine("hwclock -r") << "</td></tr>";
	Table << "</table>";
	return Table.str();
}

}
