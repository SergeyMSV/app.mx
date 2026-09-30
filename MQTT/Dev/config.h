#pragma once

#include <shareConfig.h>

#include <string>

namespace dev
{

class tDataSetConfig
{
	share::config::tPlatform m_Platform;
	share::config::tUID m_UID;
	share::config::tFamily m_Family;

public:
	explicit tDataSetConfig(const std::string& a_filename_mx);

	share::config::tPlatform GetPlatform() const { return m_Platform; }
	share::config::tUID GetUID() const { return m_UID; }
	share::config::tFamily GetFamily() const { return m_Family; }
};

}
