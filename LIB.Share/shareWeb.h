#pragma once

#include "shareConfig.h"

#include <string>

std::string GetUpdateFile(const share_config::tUpdateServer& server, const share_config::tDevice& device, const std::string& path);
