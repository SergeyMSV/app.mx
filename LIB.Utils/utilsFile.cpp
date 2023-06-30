#include "utilsFile.h"

#include <filesystem>

namespace utils
{
	namespace file
	{

std::deque<std::string> GetFilesLatest(const std::string& path, const std::string& prefix, size_t qtyFilesLatest)
{
	std::deque<std::string> List;

	std::error_code ErrCode;

	for (auto& i : std::filesystem::directory_iterator(path, ErrCode))
	{
		if (ErrCode != std::error_code())
			break;

		std::string ListFileName = i.path().filename().string();
		size_t PrefPos = ListFileName.find(prefix);
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

void RemoveFilesOutdated(const std::string& path, const std::string& prefix, size_t qtyFilesLatest)
{
	std::deque<std::string> List = GetFilesLatest(path, prefix, 0);

	while (List.size() > qtyFilesLatest)
	{
		std::error_code ErrCode;
		std::filesystem::remove(List.front(), ErrCode);
		List.pop_front();
	}
}

	}
}
