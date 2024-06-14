#pragma once

#include <devConfig.h>

#include <mutex>
#include <string>
#include <vector>

namespace dev
{

class tDataSet
{
public:
	struct tThermoItem
	{
		std::string ID;
		std::string Location;
		std::string Value;
	};
	using tThermo = std::vector<tThermoItem>;

private:
	mutable std::mutex m_mtx;
	tThermo m_Thermo;

public:
	tThermo GetThermo() const;
	void SetThermo(tThermo& value);
	void ClearThermo();
};

}
