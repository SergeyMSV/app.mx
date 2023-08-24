#pragma once

#include "shareConfig.h"

#include <deque>
#include <string>

namespace share
{

class tLogFileLine
{
	std::string m_FilePath;

public:
	tLogFileLine() = delete;
	explicit tLogFileLine(const share_config::tOutFileCap& conf);
	tLogFileLine(const tLogFileLine&) = delete;
	tLogFileLine(const tLogFileLine&&) = delete;
	~tLogFileLine();
	
	void Write(const std::string& msg);

private:
	void WriteData(const std::string& msg);
};

std::deque<std::string> GetFilePaths(const share_config::tOutFile& conf);
void RemoveFilesOutdated(const share_config::tOutFile& conf);
void RemoveFilesOutdated(const share_config::tOutFile& conf, const std::string& prefixTemp);

}