#pragma once

#include "config.h"
#include "server.h"

#include <memory>

namespace dev
{

void ThreadDALLAS(const std::shared_ptr<tConfig>& config, tTWRServer& server);

}
