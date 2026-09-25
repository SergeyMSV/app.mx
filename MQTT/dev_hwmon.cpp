#include "dev_hwmon.h"

#include <utilsPath.h>

#include <filesystem>
#include <fstream>

namespace dev
{

static std::string ReadFile(const std::filesystem::path& a_path)
{
	std::ifstream file(a_path);
	if (!file.good())
		return {};
	std::string value;
	file >> value;
	file.close();
	return value;
}

static std::uint8_t ReadFileReg(const std::filesystem::path& a_path)
{
	std::ifstream file(a_path, std::ios_base::binary);
	if (!file.good())
		return {};
	char buf[10]{};
	file.read(buf, sizeof(buf));
	std::streamsize bytesRead = file.gcount();
	file.close();
	return bytesRead == 1 ? buf[0] : 0;
}

std::vector<tHwmon> GetHwmon() // [TBD] put it in lib.share or into utilsLinux
{
	std::vector<tHwmon> res;
	const std::filesystem::path path = utils::path::GetPathNormal("/sys/class/hwmon");
	std::error_code ec;
	for (const auto& entry : std::filesystem::directory_iterator(path, ec))
	{
		if (!entry.is_directory())
			continue;
		std::string dh = entry.path().filename().string();
		if (!dh.starts_with("hwmon"))
			continue;
		tHwmon hwmon{ .ID = dh };
		for (const auto& item : std::filesystem::directory_iterator(entry.path(), ec))
		{
			if (item.is_directory())
			{
				std::string dir = item.path().filename().string();
				if (dir == "of_node")
					hwmon.Reg = ReadFileReg(item.path() / "reg");
				continue;
			}

			std::string fh = item.path().filename().string();
			if (fh == "name")
			{
				hwmon.Name = ReadFile(item.path());
			}
			else if (fh == "label")
			{
				hwmon.Label = ReadFile(item.path());
			}
			else if (fh == "temp1_input")
			{
				hwmon.Temperature = ReadFile(item.path());
			}
			else if (fh == "humidity1_input")
			{
				hwmon.Humidity = ReadFile(item.path());
			}
		}
		res.push_back(std::move(hwmon));
	}

	return res;
}

}
