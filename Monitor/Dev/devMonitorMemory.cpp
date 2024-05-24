#include "devMonitorMemory.h"

#include <utilsLinux.h>

#include <shareHTML.h>

#include <cstdlib>

#include <iomanip>
#include <sstream>
#include <string>

namespace dev
{

bool tMonitorMemory::tMemFree::IsWrong() const
{
	return Used == -1 || Free == -1 || Shared == -1 || BuffCache == -1 || Available == -1;
}

bool tMonitorMemory::tMemFree::IsEvent(const tMemFree& mem, const config::tBoardRAM& configMem) const
{
	////[TEST]
	//static int Counter = 0;
	//if (Counter++ >= 10)
	//{
	//	Counter = 0;
	//	return true;
	//}
	////------

	if (IsWrong())
		return false;//[TBD] or true ???

	if (configMem.FreeDiff > 0 && std::abs(Free - mem.Free) >= configMem.FreeDiff)
		return true;

	if (configMem.UsedDiff > 0 && std::abs(Used - mem.Used) >= configMem.UsedDiff)
		return true;

	return false;
}

void tMonitorMemory::Control(const tDataSetConfig& config)
{
	//GetMeminfo();
	GetMemFree(config);
}

bool tMonitorMemory::IsEvent() const
{
	return m_Event;
}

void tMonitorMemory::ResetEvent()
{
	m_Event = false;
}

std::string tMonitorMemory::GetHTMLTable(const std::string& styleCol1) const
{
	std::string StyleHeader = share::GetHTMLBgColour(share::tHTMLFieldStatus::TableHeader);

	std::stringstream Table;
	Table << "<table>";
	Table << "<tr><td colspan=\"2\"><b>STAT</b></td></tr>";
	Table << "<tr><td " << styleCol1 << StyleHeader << ">RAM Total</td><td>" << m_MemTotal << " kB</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Load avg</td><td>" << utils::linux::CmdLine("cat /proc/loadavg") << "</td></tr>";
	Table << "</table>";

	Table << GetHTMLTableMemFree();

	return Table.str();
}

//void tMonitorMemory::GetMeminfo()
//{
//	std::string DataRaw = utils::linux::CmdLine("cat /proc/meminfo");
//
//	std::stringstream SStr(DataRaw);
//
//	std::string StrKey;
//
//	while (!SStr.eof())
//	{
//		if (StrKey.empty())
//			SStr >> StrKey;
//		if (StrKey.empty())
//			continue;
//		if (StrKey.back() != ':')
//			continue;
//		StrKey.pop_back();
//
//		std::string StrValue;
//		if (!SStr.eof())
//			SStr >> StrValue;
//		long Value = std::strtoul(StrValue.c_str(), nullptr, 10);
//
//		std::string StrTemp;
//		if (!SStr.eof())
//			SStr >> StrTemp;
//
//		if (StrTemp.back() == ':')//it's a key
//		{
//			m_MemMap[StrKey].push_back(Value);
//			StrKey = StrTemp;
//			continue;
//		}
//		//else//it's a unit of measurement.
//		//{
//		//	calculations related to Value
//		//}
//
//		m_MemMap[StrKey].push_back(Value);
//		StrKey.clear();
//	}
//}

void tMonitorMemory::GetMemFree(const tDataSetConfig& config)
{
	std::string DataRaw = utils::linux::CmdLine("free");

	std::stringstream SStr(DataRaw);

	dev::config::tBoardRAM BoardRAMConfig = config.GetBoardRAM();

	auto GetValueNext = [&SStr]()->uint32_t
	{
		if (SStr.eof())
			return {};

		std::string StrTemp;
		SStr >> StrTemp;
		return std::strtol(StrTemp.c_str(), nullptr, 10);//in kB
	};

	while (!SStr.eof())
	{
		std::string StrTemp;
		SStr >> StrTemp;
		if (StrTemp == "Mem:")
		{
			m_MemTotal = GetValueNext();

			tMemFree Mem;			
			std::time(&Mem.Time);
			Mem.Used = GetValueNext();
			Mem.Free = GetValueNext();
			Mem.Shared = GetValueNext();
			Mem.BuffCache = GetValueNext();
			Mem.Available = GetValueNext();

			if (m_MemFree.empty())
				m_MemFree.push_back(Mem);

			if (m_MemFree.back().IsEvent(Mem, BoardRAMConfig))
			{
				m_MemFree.push_back(Mem);
				m_Event = true;
			}

			if (m_MemFree.size() > BoardRAMConfig.Qty)
				m_MemFree.pop_front();
			break;
		}
	}
}

std::string tMonitorMemory::GetHTMLTableMemFree() const
{
	std::time_t TimeNowRaw = std::time(nullptr);
	tm TimeNow = *std::gmtime(&TimeNowRaw);//gmtime may not be thread-safe.

	std::stringstream Table;
	Table << "<table>";
	Table << "<tr><td colspan=\"5\"><b>MEMORY (in kB)</b></td></tr>";
	Table << "<tr align = \"center\"" << share::GetHTMLBgColour(share::tHTMLFieldStatus::TableHeader) <<"><td>Time</td><td>Free</td><td>Used</td><td>Buff</td><td>Avail</td></tr>";
	for (auto& i : m_MemFree)
	{
		tm Time = *std::gmtime(&i.Time);//gmtime may not be thread-safe. 

		share::tHTMLFieldStatus StatFree = share::tHTMLFieldStatus::Neutral;
		share::tHTMLFieldStatus StatUsed = share::tHTMLFieldStatus::Neutral;

		if (m_MemFreePrev != tMemFree())
		{
			int DiffFree = i.Free - m_MemFreePrev.Free;
			StatFree = DiffFree == 0 ? share::tHTMLFieldStatus::Neutral : DiffFree > 0 ? share::tHTMLFieldStatus::NormAgain : share::tHTMLFieldStatus::Warn;

			int DiffUsed = i.Used - m_MemFreePrev.Used;
			StatUsed = DiffUsed == 0 ? share::tHTMLFieldStatus::Neutral : DiffUsed < 0 ? share::tHTMLFieldStatus::NormAgain : share::tHTMLFieldStatus::Warn;
		}

		Table << "<tr align = \"center\">";
		Table << "<td>";
		if (Time.tm_mday == TimeNow.tm_mday)
		{
			Table << std::put_time(&Time, "%T") << "</td>";
		}
		else
		{
			Table << std::put_time(&Time, "...-%m-%d") << "</td>"; // это вообще не так выглядит... точек нет, нуля нет
		}		
		Table << "<td" << share::GetHTMLBgColour(StatFree) << ">" << i.Free << "</td>";
		Table << "<td" << share::GetHTMLBgColour(StatUsed) << ">" << i.Used << "</td>";
		Table << "<td>" << i.BuffCache << "</td>";
		Table << "<td>" << i.Available << "</td></tr>";

		m_MemFreePrev = i;
	}
	Table << "</table>";
	return Table.str();
}

}
