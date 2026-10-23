#pragma once

#include <shareConfig.h>

namespace dev
{

class tConfig
{
	share::config::tPlatform m_Platform;
	share::config::port::tUART_Config m_Dallas;

public:
	tConfig();

	share::config::port::tUART_Config GetDallas() const { return m_Dallas; }
};

}
