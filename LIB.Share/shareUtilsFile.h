#pragma once

#include "shareConfig.h"

#include <mutex>
#include <string>

namespace share
{

#ifdef LIB_SHARE_DEPRECATED
class tLogFileLine // DEPRECATED, use utils::log::tLogFile instead
{
	std::string m_FilePath;
	mutable std::mutex m_FileMtx;

public:
	tLogFileLine() = delete;
	explicit tLogFileLine(const share::config::tOutFileCap& conf);
	tLogFileLine(const tLogFileLine&) = delete;
	tLogFileLine(const tLogFileLine&&) = delete;
	~tLogFileLine();
	
	void Write(const std::string& msg) const;
	void WriteLine(const std::string& msg) const;

private:
	void WriteData(const std::string& msg) const;
};
#endif // LIB_SHARE_DEPRECATED

void RemoveFilesOutdated(const share::config::tOutFile& conf);

bool SaveOutFile(const share::config::tOutFile& a_conf, const std::string& a_data);

}
