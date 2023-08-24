#include <devConfig.h>

#include <utilsBase.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <devDataSetConfig.h>

#include <shareWeb.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <7z2201/utilsArch_7z2201_simple.h>
#include <openssl/sha.h>

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		std::string PathFileDevice = utils::path::GetPathConfigExc("mxdevice").string();

		dev::tDataSetConfig DsConfig(PathFileConfig, PathFileDevice);
		
		std::string UpdateFileZip = GetUpdateFile(DsConfig.GetUpdateServer(), DsConfig.GetDevice(), DsConfig.GetUpdatePath());
		if (UpdateFileZip.empty())
			return static_cast<int>(utils::tExitCode::EX_OK);

		utils::arch_7z2201_simple::Decompress(UpdateFileZip);

		std::filesystem::remove(UpdateFileZip);

		std::string CmdCDPathMX = "cd " + DsConfig.GetUpdatePath() + "/mx; ";
		utils::linux::CmdLine(CmdCDPathMX + "chmod 544 setup.sh");
		utils::linux::CmdLine(CmdCDPathMX + "./setup.sh");
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}

