#include <utilsBase.h>
#include <utilsPath.h>

#include <devDataSet.h>

#include <fstream>
#include <iostream>

namespace dev
{

void MakeFile_hostname(const std::string& a_hostname)
{
	std::string Path = utils::GetPath("/etc/hostname");

	std::fstream File(Path, std::ios::out);
	if (File.good())
	{
		File << a_hostname;
		File.close();
	}
}

}
