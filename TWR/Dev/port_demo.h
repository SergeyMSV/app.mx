#pragma once

#include <devConfig.h>

#ifdef UDP_SERVER_TEST

#include "config.h"
#include "server.h"

#include <memory>

namespace dev
{

void ThreadPortDEMO(const std::shared_ptr<tConfig>& config, tTWRServer& server);

}

#endif // UDP_SERVER_TEST
