#include "devDataSet.h"

#include <utility>

namespace dev
{

tDataSet::tThermo tDataSet::GetThermo() const
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	return m_Thermo;
}

void tDataSet::SetThermo(tThermo& value)
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	m_Thermo = std::move(value);
}

void tDataSet::ClearThermo()
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	m_Thermo = {};
}

}
