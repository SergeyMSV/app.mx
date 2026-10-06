#pragma once

#include "config.h"
#include "server.h"

#include <memory>

namespace dev
{

void ThreadUART0_JSON(const std::shared_ptr<tConfig>& config, tTWRServer& server);
void ThreadUART1_JSON(const std::shared_ptr<tConfig>& config, tTWRServer& server);
void ThreadUART2_JSON(const std::shared_ptr<tConfig>& config, tTWRServer& server);
void ThreadUART3_JSON(const std::shared_ptr<tConfig>& config, tTWRServer& server);

}
