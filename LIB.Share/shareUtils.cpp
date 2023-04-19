#include "shareUtils.h"

#include <utilsLinux.h>
#include <utilsPath.h>

#include <filesystem>
#include <fstream>

namespace share
{

tLogFileLine::tLogFileLine(const share_config::tOutFileCap& conf)
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

	share::RemoveFilesOutdated(conf);
}

tLogFileLine::~tLogFileLine()
{
	WriteData("\n");
}
	
void tLogFileLine::Write(const std::string& msg)
{
	WriteData(" " + msg + ';');
}

void tLogFileLine::WriteData(const std::string& msg)
{
	std::fstream File = std::fstream(m_FilePath, std::ios::app);
	if (File.good())
		File << msg;
	File.close();
}

std::string GetFileNamePrefixTemp(std::string prefix)
{
	if (prefix.empty())
		return {};
	prefix[0] = 'X';
	return prefix;
}

std::deque<std::string> GetFilePaths(const share_config::tOutFile& conf)
{
	auto List = utils::GetFilesLatest(conf.Path, conf.Prefix, conf.QtyMax);
	utils::linux::CorrPaths(List);
	return List;
}

void RemoveFilesOutdated(const share_config::tOutFile& conf)
{
	utils::RemoveFilesOutdated(conf.Path, conf.Prefix, conf.QtyMax);
	utils::RemoveFilesOutdated(conf.Path, GetFileNamePrefixTemp(conf.Prefix), 0);
}

std::optional<std::filesystem::path> GetFilePathTemp(std::filesystem::path filePath)
{
	if (!filePath.has_filename())
		return {};
	std::string FileName = filePath.filename().string();
	if (FileName.size() < 2)
		return {};
	FileName = GetFileNamePrefixTemp(FileName);
	filePath.replace_filename(FileName);
	return filePath;
}

}
