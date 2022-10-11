#pragma once

#include "shareConfig.h"

#include <string>

namespace share
{

std::string GetHTMLTableDevice(const std::string& styleCol1);
std::string GetHTMLTableSystem(const std::string& styleCol1, const share_config::tDevice& dsConfigDevice);

}
