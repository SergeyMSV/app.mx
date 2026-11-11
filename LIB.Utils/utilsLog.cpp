#include "utilsLog.h"
#include "utilsFile.h"

#include <chrono>
#include <format>
#include <iomanip>

#ifndef LIB_UTILS_LOG_DATETIME_PATH_FORMAT
#define LIB_UTILS_LOG_DATETIME_PATH_FORMAT "%Y-%m-%d_%H-%M-%S"
#endif // LIB_UTILS_LOG_DATETIME_PATH_FORMAT

#define LIB_UTILS_LOG_DATETIME_PATH_FORMAT2 "{:" LIB_UTILS_LOG_DATETIME_PATH_FORMAT "}"

namespace utils
{
namespace log
{

#ifdef LIB_UTILS_LOG

#ifdef LIB_UTILS_LOG_COLOR
static std::string ToString(tColor a_color)
{
	std::string color = "\x1b[";
	switch (a_color)
	{
	case tColor::None:         color += "0";  break;
	case tColor::Black:        color += "30"; break;
	case tColor::Red:          color += "31"; break;
	case tColor::Green:        color += "32"; break;
	case tColor::Yellow:       color += "33"; break;
	case tColor::Blue:         color += "34"; break;
	case tColor::Magenta:      color += "35"; break;
	case tColor::Cyan:         color += "36"; break;
	case tColor::White:        color += "37"; break;
	case tColor::Default:      color += "39"; break;
	case tColor::LightGray:    color += "90"; break;
	case tColor::LightRed:     color += "91"; break;
	case tColor::LightGreen:   color += "92"; break;
	case tColor::LightYellow:  color += "93"; break;
	case tColor::LightBlue:    color += "94"; break;
	case tColor::LightMagenta: color += "95"; break;
	case tColor::LightCyan:    color += "96"; break;
	case tColor::LightWhite:   color += "97"; break;
	default:                   color += "39"; break;
	}
	color += "m";
	return color;
}
#endif //LIB_UTILS_LOG_COLOR

void tLog::Write(bool timestamp, const std::string& msg, tColor color)
{
	WriteLog(timestamp, false, msg, color);
}

void tLog::Write(bool timestamp, const std::string& msg)
{
	Write(timestamp, msg, tColor::Default);
}

#ifdef LIB_UTILS_LOG_DEPRECATED
void tLog::Write(bool timestamp, tColor colorText, const std::string& msg)
{
	Write(timestamp, msg, colorText);
}
#endif // LIB_UTILS_LOG_DEPRECATED

void tLog::WriteLine()
{
	WriteLog(false, true, "", tColor::Default);
}

void tLog::WriteLine(bool timestamp, const std::string& msg, tColor color)
{
	WriteLog(timestamp, true, msg, color);
}

void tLog::WriteLine(bool timestamp, const std::string& msg)
{
	WriteLog(timestamp, true, msg, tColor::Default);
}

void tLog::WriteLine(const std::string& msg, tColor color)
{
	WriteLog(true, true, msg, color);
}

void tLog::WriteLine(const std::string& msg)
{
	WriteLog(true, true, msg, tColor::Default);
}

#ifdef LIB_UTILS_LOG_DEPRECATED
void tLog::WriteLine(bool timestamp, tColor colorText, const std::string& msg)
{
	WriteLine(timestamp, msg, colorText);
}
#endif // LIB_UTILS_LOG_DEPRECATED

static bool IsSymbol(char value)
{
	return value > 0x20 && value != 0x25 && value != 0x7F;
}

template <typename T>
static std::enable_if<std::is_same<T, char>::value || std::is_same<T, unsigned char>::value, std::string>::type MakeStringHex(const std::vector<T>& data, int dataLinesBegin, int dataLinesEnd)
{
	const int LinesQty = static_cast<int>((data.size() / 16) + (data.size() % 16 ? 1 : 0));
	const int LinesToSkip = dataLinesBegin && dataLinesEnd ? static_cast<int>(LinesQty - dataLinesBegin - dataLinesEnd) : 0;
	if (LinesToSkip > 0)
	{
		--dataLinesBegin; // transform to index
		dataLinesEnd = LinesQty - dataLinesEnd;
	}

	const auto IsLineSkipped = [&dataLinesBegin, &dataLinesEnd, &LinesToSkip](int linesCounter)->bool { return LinesToSkip > 0 && linesCounter > dataLinesBegin && linesCounter < dataLinesEnd; };

	std::stringstream Stream;
	bool SkipMsgInserted = false;
	for (int i = 0; i < LinesQty; ++i)
	{
		if (IsLineSkipped(i))
		{
			if (!SkipMsgInserted)
			{
				SkipMsgInserted = true;
				Stream << "< " + std::to_string(LinesToSkip) + " lines are skipped >\n";
			}
			continue;
		}

		Stream << std::setfill('0') << std::setw(4) << std::hex << (16 * i) << "   ";

		std::string Substr;
		std::size_t byteCount = 0;
		for (std::size_t k = (16 * i); byteCount < 16 && k < data.size(); ++byteCount, ++k)
		{
			if (byteCount == 8)
				Stream << "  ";

			Stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(data[k]) << ' ';

			Substr += IsSymbol(data[k]) ? data[k] : '.';
		}
		for (; byteCount < 16; ++byteCount)
		{
			if (byteCount == 8)
				Stream << "  ";
			Stream << "   ";
		}
		Stream << "  " + Substr;
		

		if (i < LinesQty - 1) // It's not needed for the last string
			Stream << '\n';
	}

	return Stream.str();
}

#ifdef LIB_UTILS_LOG_DEPRECATED
void tLog::WriteHex(bool timestamp, tColor colorText, const std::string& msg, const std::vector<std::uint8_t>& data)
{
	WriteHex(timestamp, msg, colorText, data, colorText);
}
#endif // LIB_UTILS_LOG_DEPRECATED

void tLog::WriteHex(const std::vector<std::uint8_t>& data, tColor dataColor, int dataLinesBegin, int dataLinesEnd)
{
	WriteLog(false, true, MakeStringHex(data, dataLinesBegin, dataLinesEnd), dataColor);
}

void tLog::WriteHex(const std::vector<std::uint8_t>& data, tColor dataColor)
{
	WriteHex(data, dataColor, 0, 0);
}

void tLog::WriteHex(const std::vector<std::uint8_t>& data)
{
	WriteHex(data, tColor::Default, 0, 0);
}

void tLog::WriteHex(bool timestamp, const std::string& msg, tColor msgColor, const std::vector<std::uint8_t>& data, tColor dataColor, int dataLinesBegin, int dataLinesEnd)
{
	WriteLog(timestamp, true, msg, msgColor);
	WriteHex(data, dataColor, dataLinesBegin, dataLinesEnd);
}

void tLog::WriteHex(bool timestamp, const std::string& msg, tColor msgColor, const std::vector<std::uint8_t>& data, tColor dataColor)
{
	WriteHex(timestamp, msg, msgColor, data, dataColor, 0, 0);
}

void tLog::WriteHex(bool timestamp, const std::string& msg, const std::vector<std::uint8_t>& data, tColor color, int dataLinesBegin, int dataLinesEnd)
{
	WriteHex(timestamp, msg, color, data, color, dataLinesBegin, dataLinesEnd);
}

void tLog::WriteHex(bool timestamp, const std::string& msg, const std::vector<std::uint8_t>& data, tColor color)
{
	WriteHex(timestamp, msg, color, data, color);
}

void tLog::WriteHex(bool timestamp, const std::string& msg, const std::vector<std::uint8_t>& data, int dataLinesBegin, int dataLinesEnd)
{
	WriteHex(timestamp, msg, tColor::Default, data, tColor::Default, dataLinesBegin, dataLinesEnd);
}

void tLog::WriteHex(bool timestamp, const std::string& msg, const std::vector<std::uint8_t>& data)
{
	WriteHex(timestamp, msg, tColor::Default, data, tColor::Default);
}

template<typename Unit>
static std::string GetTimestamp()
{
	auto timeNowRaw = std::chrono::system_clock::now();
	auto timeNow = std::chrono::time_point_cast<Unit>(timeNowRaw);
	return std::format("{:%T}", timeNow);
}

void tLog::WriteLog(bool a_timestamp, bool a_endl, const std::string& a_text, tColor a_textColor)
{
	std::string strHeader;
	if (a_timestamp)
	{
#ifdef LIB_UTILS_LOG_TIMESTAMP_MICROSECONDS
		strHeader = GetTimestamp<std::chrono::microseconds>();
#else // LIB_UTILS_LOG_TIMESTAMP_MICROSECONDS
		strHeader = GetTimestamp<std::chrono::milliseconds>();
#endif // LIB_UTILS_LOG_TIMESTAMP_MICROSECONDS
		strHeader += ' ';
	}

	std::lock_guard<std::mutex> Lock(m_Mtx);

	const std::string strNoColor = strHeader + a_text + (a_endl ? "\n" : ""); // The text is not colored.
	WriteLogFile(strNoColor);
#ifndef LIB_UTILS_LOG_COLOR
	WriteLog(strNoColor);
#else // LIB_UTILS_LOG_COLOR
	const std::string str = strHeader + ToString(a_textColor) + a_text + ToString(tColor::None) + (a_endl ? "\n" : "");
	WriteLog(str);
#endif // LIB_UTILS_LOG_COLOR
}

#endif // LIB_UTILS_LOG

#ifdef LIB_UTILS_LOG_FILE

static std::string GetTimestampPath()
{
	auto timeNowRaw = std::chrono::system_clock::now();
	auto timeNow = std::chrono::time_point_cast<std::chrono::seconds>(timeNowRaw);
	return std::format(LIB_UTILS_LOG_DATETIME_PATH_FORMAT2, timeNow);
}

tLogFile::tFile::tFile(tLogFile& a_obj)
{
	bool isFileNew = false;
	if (a_obj.m_FilePath.empty())
	{
		auto LogList = utils::file::GetFilesLatest(a_obj.m_FilesPath, a_obj.m_FilenamePrefix, m_FileExtension, a_obj.m_FilesQtyMax);
		std::string filePath = LogList.empty() ? "" : LogList.back();
		if (IsFileAcceptable(filePath, a_obj))
		{
			a_obj.m_FilePath = filePath;
		}
		else
		{
			a_obj.m_FilePath = MakeFilePath(a_obj.m_FilesPath, a_obj.m_FilenamePrefix);
			isFileNew = true;
		}
	}
	else if (!IsFileAcceptable(a_obj.m_FilePath, a_obj))
	{
		a_obj.m_FilePath = MakeFilePath(a_obj.m_FilesPath, a_obj.m_FilenamePrefix);
		isFileNew = true;
	}

	m_File = std::fstream(a_obj.m_FilePath, std::ios::app);

	if (isFileNew)
		utils::file::RemoveFilesOutdated(a_obj.m_FilesPath, a_obj.m_FilenamePrefix, m_FileExtension, a_obj.m_FilesQtyMax);
}

tLogFile::tFile::~tFile()
{
	m_File.close();
}

void tLogFile::tFile::Write(const std::string& a_text)
{
	if (m_File.good())
		m_File << a_text;
}

bool tLogFile::tFile::IsFileAcceptable(const std::filesystem::path& a_filepath, const tLogFile& a_obj) const
{
	if (a_filepath.empty())
		return false;
	auto fileSize = std::filesystem::file_size(a_filepath);
	if (fileSize >= a_obj.m_FileCapacity)
		return false;
	std::string filename = a_filepath.filename().replace_extension().string();
	if (!filename.starts_with(a_obj.m_FilenamePrefix))
		return false;
	filename.erase(0, a_obj.m_FilenamePrefix.size());
	std::chrono::sys_seconds tp;
	std::istringstream iss(filename);
	iss >> std::chrono::parse(LIB_UTILS_LOG_DATETIME_PATH_FORMAT, tp);
	if (iss.fail())
		return false;
	auto dateFile = std::chrono::floor<std::chrono::days>(tp);
	auto dateNowRaw = std::chrono::system_clock::now();
	auto dateNow = std::chrono::floor<std::chrono::days>(dateNowRaw);
	return dateFile == dateNow;	
}

std::filesystem::path tLogFile::tFile::MakeFilePath(const std::string& a_filesPath, const std::string& a_filenamePrefix) const
{
	std::filesystem::path path = a_filesPath;
	std::string filename = a_filenamePrefix + GetTimestampPath() + m_FileExtension;
	return path / filename;
}

tLogFile::tLogFile(const std::string& filesPath, std::uint32_t filesQtyMax, const std::string& fileNamePrefix, std::uint32_t fileCapacity)
	:m_FilesPath(filesPath), m_FilesQtyMax(filesQtyMax), m_FilenamePrefix(fileNamePrefix), m_FileCapacity(fileCapacity)
{
}

void tLogFile::WriteLogFile(const std::string& a_text)
{
	tFile(*this).Write(a_text);
}

#endif // LIB_UTILS_LOG_FILE

std::string GetLogMessage(const std::string& msg, const std::string& filename, int line)
{
	const std::filesystem::path Path(filename); // C++17
	return msg + " " + Path.filename().string() + ":" + std::to_string(line);
}

}
}
