#pragma once

#include <devConfig.h>

#include <mutex>
#include <string>
#include <vector>

namespace dev
{

class tDataSet
{
	mutable std::mutex m_mtx;
	card_mfr::tCardUL m_Card;

public:
	card_mfr::tCardUL GetCard() const;
	void SetCard(const card_mfr::tCardUL& value);
	void ClearCard();
};

}
