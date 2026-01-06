#include "devDataSet.h"

namespace dev
{

utils::mifare::tCardUL tDataSet::GetCard() const
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	return m_Card;
}

void tDataSet::SetCard(const utils::mifare::tCardUL& value)
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	m_Card = value;
}

void tDataSet::ClearCard()
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	m_Card = utils::mifare::tCardUL();
}

}
