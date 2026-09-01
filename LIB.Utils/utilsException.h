///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsException
// 2025-03-24
// C++17
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <stdexcept>
#ifdef LIB_UTILS_BASE_DEPRECATED
#include <string>

namespace utils
{
namespace log_ex
{

std::string GetLogMessage(const std::string& msg, const std::string& filename, int line);

}
}

#define THROW_INVALID_ARGUMENT(msg) throw std::invalid_argument{ utils::log_ex::GetLogMessage(msg, __FILE__, __LINE__) }
#define THROW_RUNTIME_ERROR(msg) throw std::runtime_error{ utils::log_ex::GetLogMessage(msg, __FILE__, __LINE__) }
#else // LIB_UTILS_BASE_DEPRECATED
#include "utilsLog.h"

#define THROW_INVALID_ARGUMENT(msg) throw std::invalid_argument{ utils::log::GetLogMessage(msg, __FILE__, __LINE__) }
#define THROW_RUNTIME_ERROR(msg) throw std::runtime_error{ utils::log::GetLogMessage(msg, __FILE__, __LINE__) }
#endif // LIB_UTILS_BASE_DEPRECATED