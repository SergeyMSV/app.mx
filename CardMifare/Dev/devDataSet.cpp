#include "devDataSet.h"

namespace dev
{

utils::card_MIFARE::ultralight::tCard tDataSet::GetCard() const
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	return m_Card;
}

void tDataSet::SetCard(const utils::card_MIFARE::ultralight::tCard& value)
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	m_Card = value;
}

void tDataSet::ClearCard()
{
	std::lock_guard<std::mutex> Lock(m_mtx);
	m_Card = utils::card_MIFARE::ultralight::tCard();
}

}
