#pragma once

#include "shareConfig.h"

#include <deque>
#include <mutex>
#include <string>

namespace share
{

class tLogFileLine
{
	std::string m_FilePath;
	std::mutex m_FileMtx;

public:
	tLogFileLine() = delete;
	explicit tLogFileLine(const share::config::tOutFileCap& conf);
	tLogFileLine(const tLogFileLine&) = delete;
	tLogFileLine(const tLogFileLine&&) = delete;
	~tLogFileLine();
	
	void Write(const std::string& msg);

private:
	void WriteData(const std::string& msg);
};

std::deque<std::string> GetFilePaths(const share::config::tOutFile& conf);
void RemoveFilesOutdated(const share::config::tOutFile& conf);
void RemoveFilesOutdated(const share::config::tOutFile& conf, const std::string& prefixTemp);

}