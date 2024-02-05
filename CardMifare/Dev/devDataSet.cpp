#include "devDataSet.h"

namespace dev
{

	card_mfr::tCardUL tDataSet::GetCard() const
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	return m_Card;
}

void tDataSet::SetCard(const card_mfr::tCardUL& value)
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	m_Card = value;
}

void tDataSet::ClearCard()
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	m_Card = card_mfr::tCardUL();
}

}
