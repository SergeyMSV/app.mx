#pragma once

#include <devConfig.h>
#include <utilsMIFARE.h>

#include <mutex>
#include <string>
#include <vector>

namespace dev
{

class tDataSet
{
	mutable std::mutex m_mtx;
	utils::mifare::tCardUL m_Card;

public:
	utils::mifare::tCardUL GetCard() const;
	void SetCard(const utils::mifare::tCardUL& value);
	void ClearCard();
};

}
