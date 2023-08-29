#include <utilsBase.h>
#include <utilsLinux.h>

#include <devDataSetConfig.h>

#include <fstream>
#include <string>

constexpr char g_PathFstab[] = "/etc/fstab";

std::vector<std::string> ReadFileLines(const std::string& path)
{
	std::vector<std::string> Lines;
	std::fstream File(path, std::ios::in);
	if (File.good())
	{
		while (!File.eof())
		{
			char Str[128];
			File.getline(Str, sizeof(Str), 0x0A);//LF
			if (Str[0])
				Lines.push_back(Str);
		}

		File.close();
	}
	return Lines;
}

void WriteFileLines(const std::string& path, const std::vector<std::string>& lines)
{
	std::fstream File = std::fstream(path, std::ios::out | std::ios::binary);
	if (File.good())
	{
		for (auto& i : lines)
		{
			File.write(i.c_str(), i.size());
			File.put(0x0A);
		}
		File.close();
	}
}

bool SetupFstab(const dev::config::tFstab& fstabConf)
{
	std::string PathFileFstab = utils::linux::GetPath(g_PathFstab);
	std::vector<std::string> Lines = ReadFileLines(PathFileFstab);

	const std::string Line = "tmpfs          " + fstabConf.fs + "  tmpfs  nosuid,nodev,noatime,size=" + fstabConf.size + "   0  0";

	auto Iter = std::find_if(Lines.begin(), Lines.end(), [&](const std::string& val) {return std::string::npos != val.find(fstabConf.fs); });
	if (Iter != Lines.end())
	{
		if (*Iter == Line)
			return false;

		Lines.erase(Iter);
	}

	Lines.push_back(Line);

	WriteFileLines(PathFileFstab, Lines);
	return true;
}

bool RevertFstab(const dev::config::tFstab& fstabConf)
{
	std::string PathFileFstab = utils::linux::GetPath(g_PathFstab);
	std::vector<std::string> Lines = ReadFileLines(PathFileFstab);

	auto Iter = std::find_if(Lines.begin(), Lines.end(), [&](const std::string& val) {return std::string::npos != val.find(fstabConf.fs); });
	if (Iter != Lines.end())
	{
		Lines.erase(Iter);

		WriteFileLines(PathFileFstab, Lines);
		return true;
	}
	return false;
}
