#pragma once

#include "config.h"
#include "server.h"

#include <memory>

namespace dev
{

void ThreadPortSPI0_CS0(const std::shared_ptr<tConfig>& config, tTWRServer& server);

}
