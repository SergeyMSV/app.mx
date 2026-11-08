///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsLog
// 2016-05-16 (before) - 2026-11-06
// C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <libConfig.h>

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>

#include <cstdint>

#ifdef LIB_UTILS_LOG_FILE
#define LIB_UTILS_LOG
#endif // LIB_UTILS_LOG_FILE

namespace utils
{
namespace log
{

enum class tColor
{
	None,
	Black,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
	Default,
	LightGray,
	LightRed,
	LightGreen,
	LightYellow,
	LightBlue,
	LightMagenta,
	LightCyan,
	LightWhite,
};

#ifdef LIB_UTILS_LOG
#define LIB_UTILS_IMPL ;
#else // LIB_UTILS_LOG
#define LIB_UTILS_IMPL {}
#endif // LIB_UTILS_LOG

class tLog
{
	mutable std::mutex m_Mtx;

public:
	tLog() = default;
	virtual ~tLog() {}

	void Write(bool timestamp, const std::string& msg, tColor color) LIB_UTILS_IMPL
	void Write(bool timestamp, const std::string& msg) LIB_UTILS_IMPL
#ifdef LIB_UTILS_LOG_DEPRECATED
	void Write(bool timestamp, tColor colorText, const std::string& msg) LIB_UTILS_IMPL // DEPRECATED
#endif // LIB_UTILS_LOG_DEPRECATED

	void WriteLine() LIB_UTILS_IMPL
	void WriteLine(bool timestamp, const std::string& msg, tColor color) LIB_UTILS_IMPL
	void WriteLine(bool timestamp, const std::string& msg) LIB_UTILS_IMPL
	void WriteLine(const std::string& msg, tColor color) LIB_UTILS_IMPL
	void WriteLine(const std::string& msg) LIB_UTILS_IMPL
#ifdef LIB_UTILS_LOG_DEPRECATED
	void WriteLine(bool timestamp, tColor colorText, const std::string& msg) LIB_UTILS_IMPL // DEPRECATED

	void WriteHex(bool timestamp, tColor colorText, const std::string& msg, const std::vector<std::uint8_t>& data) LIB_UTILS_IMPL // DEPRECATED
#endif // LIB_UTILS_LOG_DEPRECATED

	void WriteHex(const std::vector<std::uint8_t>& data, tColor dataColor, int dataLinesBegin, int dataLinesEnd) LIB_UTILS_IMPL
	void WriteHex(const std::vector<std::uint8_t>& data, tColor dataColor) LIB_UTILS_IMPL
	void WriteHex(const std::vector<std::uint8_t>& data) LIB_UTILS_IMPL
	void WriteHex(bool timestamp, const std::string& msg, tColor msgColor, const std::vector<std::uint8_t>& data, tColor dataColor, int dataLinesBegin, int dataLinesEnd)  LIB_UTILS_IMPL
	void WriteHex(bool timestamp, const std::string& msg, tColor msgColor, const std::vector<std::uint8_t>& data, tColor dataColor) LIB_UTILS_IMPL
	void WriteHex(bool timestamp, const std::string& msg, const std::vector<std::uint8_t>& data, tColor color, int dataLinesBegin, int dataLinesEnd) LIB_UTILS_IMPL
	void WriteHex(bool timestamp, const std::string& msg, const std::vector<std::uint8_t>& data, tColor color) LIB_UTILS_IMPL
	void WriteHex(bool timestamp, const std::string& msg, const std::vector<std::uint8_t>& data, int dataLinesBegin, int dataLinesEnd) LIB_UTILS_IMPL
	void WriteHex(bool timestamp, const std::string& msg, const std::vector<std::uint8_t>& data) LIB_UTILS_IMPL

protected:
	virtual void WriteLog(const std::string& a_text) = 0;
	virtual void WriteLogFile(const std::string& a_text) {}

private:
	virtual void WriteLog(bool a_timestamp, bool a_endl, const std::string& a_text, tColor a_textColor) LIB_UTILS_IMPL
};

#ifdef LIB_UTILS_LOG_FILE

class tLogFile : public tLog
{
	static constexpr char m_FileExtension[] = ".log";

	class tFile
	{
		std::fstream m_File;

	public:
		explicit tFile(tLogFile& a_obj);
		~tFile();

		bool IsReady() const { return m_File.good(); }
		void Write(const std::string& a_text);

	private:
		bool IsFileAcceptable(const std::filesystem::path& a_filepath, const tLogFile& a_obj) const;
		std::filesystem::path MakeFilePath(const std::string& a_filesPath, const std::string& a_filenamePrefix) const;
	};

	const std::string m_FilesPath;
	const std::uint32_t m_FilesQtyMax = 0;
	const std::string m_FilenamePrefix;
	const std::uint32_t m_FileCapacity; // in bytes

	std::filesystem::path m_FilePath;
	//mutable std::mutex m_FileMtx;

public:
	tLogFile() = delete;
	tLogFile(const std::string& filesPath, std::uint32_t filesQtyMax, const std::string& filenamePrefix, std::uint32_t fileCapacity);
	tLogFile(const tLogFile&) = delete;
	tLogFile(const tLogFile&&) = delete;

protected:
	void WriteLog(const std::string& a_text) override {}
	void WriteLogFile(const std::string& a_text) override final;
};

#endif // LIB_UTILS_LOG_FILE

std::string GetLogMessage(const std::string& msg, const std::string& filename, int line);

}
}
