#include "shareUtilsFile.h"

#include <utilsFile.h>
#include <utilsPath.h>
#include <utilsTime.h>

#include <filesystem>
#include <fstream>
#ifdef GNSS_TEST
#include <iostream>
#endif // GNSS_TEST

namespace share
{

tLogFileLine::tLogFileLine(const share::config::tOutFileCap& conf)
{
	auto LogList = utils::file::GetFilesLatest(conf.Path, conf.Prefix, ".log", conf.QtyMax);
	m_FilePath = LogList.empty() ? "" : LogList.back();

	std::fstream File;

	if (!m_FilePath.empty())
	{
		auto FileSize = std::filesystem::file_size(m_FilePath);
		if (FileSize < conf.Capacity)
			File = std::fstream(m_FilePath, std::ios::app);
	}

	const std::string DTStr = utils::time::tDateTime::Now().ToStringPath();

	if (!File.is_open())
	{
		std::string Path = conf.Path + "/";
		std::string FileName = conf.Prefix + DTStr + ".log";

		m_FilePath = Path + FileName;
		File = std::fstream(m_FilePath, std::ios::out);
	}

	if (File.good())
		File << DTStr << " ";

	File.close();

	utils::file::RemoveFilesOutdated(conf.Path, conf.Prefix, ".log", conf.QtyMax);
}

tLogFileLine::~tLogFileLine()
{
	WriteData("\n");
}
	
void tLogFileLine::Write(const std::string& msg)
{
	WriteData(" " + msg + ';');
}

void tLogFileLine::WriteLine(const std::string& msg)
{
	WriteData(msg + '\n');
}

void tLogFileLine::WriteData(const std::string& msg)
{
	std::lock_guard<std::mutex> Lock(m_FileMtx);
	std::fstream File = std::fstream(m_FilePath, std::ios::app);
	if (File.good())
		File << msg;
	File.close();
#ifdef GNSS_TEST
	std::cout << msg;
#endif // GNSS_TEST
}

void RemoveFilesOutdated(const share::config::tOutFile& conf)
{
	if (conf.IsWrong())
		return;
	utils::file::RemoveFilesOutdated(conf.Path, conf.Prefix, ".json", conf.QtyMax);
	utils::file::RemoveFilesOutdated(conf.Path, "_" + conf.Prefix, ".tmp", 0); // It removes temp-files.
}

bool SaveOutFile(const share::config::tOutFile& a_conf, const std::string& a_data)
{
	const std::string dtStr = utils::time::tDateTime::Now().ToStringPath();

	const std::filesystem::path path = a_conf.Path;
	const std::string filename = a_conf.Prefix + dtStr + ".json";
	const std::string filenameTemp = "_" + filename + ".tmp";

	std::filesystem::path filepathTemp = path / filenameTemp;
	std::fstream File = std::fstream(filepathTemp, std::ios::out);
	if (!File.good())
		return false;
	File << a_data;
	File.close();

	std::filesystem::path filepath = path / filename;
	std::error_code ec;
	std::filesystem::rename(filepathTemp, filepath, ec);
	share::RemoveFilesOutdated(a_conf);
	return true;
}

}
