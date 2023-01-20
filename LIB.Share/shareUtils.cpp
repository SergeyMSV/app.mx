#include "shareUtils.h"

#include <utilsLinux.h>
#include <utilsPath.h>

#include <filesystem>
#include <fstream>

namespace share
{

tLogFile::tLogFile(const share_config::tOutFileCap& conf)
{
	auto LogList = GetFilePaths(conf);
	m_FilePath = LogList.empty() ? "" : LogList.back();

	std::fstream File;

	if (!m_FilePath.empty())
	{
		auto FileSize = std::filesystem::file_size(m_FilePath);
		if (FileSize < conf.Capacity)
			File = std::fstream(m_FilePath, std::ios::app);
	}

	if (!File.is_open())
	{
		std::string DTStr = utils::GetDateTime();
		std::string Path = conf.Path + "/";
		std::string FileName = conf.Prefix + DTStr + ".log";

		m_FilePath = Path + FileName;
		File = std::fstream(m_FilePath, std::ios::out);
	}

	std::string DTStr = utils::GetDateTime();
	if (File.good())
		File << DTStr;

	File.close();
}

tLogFile::~tLogFile()
{
	WriteData("\n");
}
	
void tLogFile::Write(const std::string& msg)
{
	WriteData(" " + msg + ';');
}

void tLogFile::WriteData(const std::string& msg)
{
	std::fstream File = std::fstream(m_FilePath, std::ios::app);
	if (File.good())
		File << msg;
	File.close();
}

std::deque<std::string> GetFilePaths(const share_config::tOutFile& conf)
{
	utils::RemoveFilesOutdated(conf.Path, conf.Prefix, conf.QtyMax);
	auto List = utils::GetFilesLatest(conf.Path, conf.Prefix, conf.QtyMax);
	utils::linux::CorrPaths(List);
	return List;
}

}
