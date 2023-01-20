#pragma once

#include <utilsBase.h>

#include "shareConfig.h"

#include <deque>
#include <string>

namespace share
{

class tLogFile
{
	std::string m_FilePath;

public:
	tLogFile() = delete;
	explicit tLogFile(const share_config::tOutFileCap& conf);
	tLogFile(const tLogFile&) = delete;
	tLogFile(const tLogFile&&) = delete;
	~tLogFile();
	
	void Write(const std::string& msg);

private:
	void WriteData(const std::string& msg);
};

std::deque<std::string> GetFilePaths(const share_config::tOutFile& conf);

}