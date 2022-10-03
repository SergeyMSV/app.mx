#pragma once

#include "utilsBase.h"

#include <optional>
#include <string>

namespace utils
{
	namespace web
	{

std::optional<std::string> HttpsReqSyncString(const std::string& a_host, const std::string& a_target);
std::optional<tVectorUInt8> HttpsReqSyncVector(const std::string& a_host, const std::string& a_target);

	}
}