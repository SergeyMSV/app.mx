#pragma once

#include <utilsBase.h>

#include <deque>
#include <string>
#include <utility>
	
struct tUpdateItem
{
	std::string Head;
	std::string RefValue;
	std::string SHA256;
};
using tUpdateList = std::deque<tUpdateItem>;
