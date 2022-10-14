#pragma once

#include <utilsBase.h>

#include "devDataSetConfig.h"

#include <deque>
#include <map>
#include <string>
#include <vector>

namespace dev
{

class tMonitorMemory
{
	struct tMemFree
	{
		time_t Time = 0;
		int32_t Used = -1;//in kB
		int32_t Free = -1;//in kB
		int32_t Shared = -1;//in kB
		int32_t BuffCache = -1;//in kB
		int32_t Available = -1;//in kB

		tMemFree() = default;

		bool IsWrong() const;
		bool IsEvent(const tMemFree& mem, const config::tBoardRAM& configMem) const;

		bool operator==(const tMemFree&) const = default;
		bool operator!=(const tMemFree&) const = default;
	};

	int32_t m_MemTotal = -1;//in kB
	std::deque<tMemFree> m_MemFree;
	mutable tMemFree m_MemFreePrev;
	//std::map<std::string, std::vector<int>> m_MemMap;

	bool m_Event = false;

public:
	tMonitorMemory() = default;

	void Control(const tDataSetConfig& config);//[TBD] might be is not perfect, it might be better to use thread inside

	//bool IsAlert();
	bool IsEvent() const;
	void ResetEvent();

	std::string GetHTMLTable(const std::string& styleCol1) const;

	//size_t size() { return m_MemFree.size(); }
	//tMemFree operator[](int index) { return m_MemFree[index]; }

private:
	//void GetMeminfo();
	void GetMemFree(const tDataSetConfig& config);

	std::string GetHTMLTableMemFree() const;
};

}