#if defined(_WIN32)

#include <filesystem>
#include <fstream>
#include <string>

#include "utilsPath.h"

namespace utils
{
namespace linux
{

std::string CmdLineWinTest(const std::string& cmd)
{
	if (cmd == "hostname")
	{
		std::filesystem::path Path = path::GetPathNormal("/etc/hostname");
		if (Path.empty())
			return {};
		std::ifstream File(Path);
		if (!File.good())
			return {};
		std::string Data;
		while (!File.eof())
		{
			std::string Line;
			std::getline(File, Line);
			Data += Line + "\n";
		}
		File.close();
		return Data;
	}
	return {};
}

}
}

#endif // _WIN32
