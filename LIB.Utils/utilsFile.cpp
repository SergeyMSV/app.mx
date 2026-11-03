#include "utilsFile.h"

#include <filesystem>

namespace utils
{
	namespace file
	{

std::optional<std::string> ReplaceFileNamePrefix(const std::string& path, const std::string& from, const std::string& to)
{
	const std::size_t PosSlash = path.find_last_of('/');
	const bool IsPath = PosSlash != std::string::npos;
	const std::size_t Pos = path.find(from, IsPath ? PosSlash : 0);

	if (Pos == std::string::npos || IsPath && Pos != PosSlash + 1)
		return std::nullopt;

	std::string PathRes = path;
	return PathRes.replace(Pos, from.length(), to);
}

std::deque<std::string> GetFilesLatest(const std::string& path, const std::string& prefix, std::string extension, std::size_t qtyFilesLatest)
{
	if (!extension.empty() && extension[0] != '.')
		extension.insert(extension.begin(), '.');

	std::deque<std::string> List;

	std::error_code ErrCode;

	for (auto& i : std::filesystem::directory_iterator(path, ErrCode))
	{
		if (ErrCode != std::error_code())
			break;
		std::filesystem::path Filename = i.path().filename();
		if (!extension.empty() && Filename.extension().string() != extension)
			continue;
		std::size_t PrefPos = Filename.string().find(prefix);
		if (PrefPos != 0)
			continue;
		List.push_back(i.path().string());
	}

	if (qtyFilesLatest != 0)
	{
		std::sort(List.begin(), List.end(), [](const std::string& a, const std::string& b) { return a > b; });

		if (List.size() > qtyFilesLatest)
			List.resize(qtyFilesLatest);
	}

	std::sort(List.begin(), List.end());

	return List;
}

void RemoveFilesOutdated(const std::string& path, const std::string& prefix, std::string extension, std::size_t qtyFilesLatest)
{
	std::deque<std::string> List = GetFilesLatest(path, prefix, extension, {});

	while (List.size() > qtyFilesLatest)
	{
		std::error_code ErrCode;
		std::filesystem::remove(List.front(), ErrCode);
		List.pop_front();
	}
}

void RemoveFilesWithPrefix(const std::string& path, const std::string& prefix)
{
	std::deque<std::string> List = GetFilesLatest(path, prefix, {}, {});

	while (!List.empty())
	{
		std::error_code ErrCode;
		std::filesystem::remove(List.front(), ErrCode);
		List.pop_front();
	}
}

	}
}
