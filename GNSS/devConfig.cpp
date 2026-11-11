#include "devConfig.h"

#ifdef GNSS_TEST

#include <iostream>

tLog::tLog(const std::string& filesPath, std::uint32_t filesQtyMax, const std::string& filenamePrefix, std::uint32_t fileCapacity)
	:tLogFile(filesPath, filesQtyMax, filenamePrefix, fileCapacity)
{
}

void tLog::WriteLog(const std::string& a_text)
{
	std::cout << a_text;
}

#endif // GNSS_TEST
