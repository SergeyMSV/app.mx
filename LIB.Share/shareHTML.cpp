#include "shareConfig.h"
#include "shareHTML.h"

#include <utilsLinux.h>

#include <sstream>
#include <string>

namespace share
{

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

std::string GetHTMLTableSystem(const std::string& styleCol1, const share_config::tDevice& dsConfigDevice)
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

}
