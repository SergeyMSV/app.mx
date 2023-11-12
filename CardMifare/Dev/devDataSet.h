#pragma once

#include <utilsCardMIFARE.h>

#include <mutex>
#include <string>
#include <vector>

namespace dev
{

class tDataSet
{
	mutable std::mutex m_mtx;
	utils::card_MIFARE::ultralight::tCard m_Card;

public:
	utils::card_MIFARE::ultralight::tCard GetCard() const;
	void SetCard(const utils::card_MIFARE::ultralight::tCard& value);
	void ClearCard();
};

}
