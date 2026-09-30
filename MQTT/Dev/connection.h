#pragma once

#include "config.h"

#include <memory>
#include <string_view>

namespace dev
{

void TaskConnectionHandler(std::string_view host, std::string_view service, std::shared_ptr<dev::tDataSetConfig> сonfig);

}
