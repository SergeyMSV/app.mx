#include <utilsBase.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <devDataSetConfig.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

bool MakeFile_fstab();
void MakeFile_msmtprc(const std::string& a_path, const dev::config::tEmail& a_email);
void MakeFile_muttrc(const std::string& a_path, const std::string& a_hostname, const dev::config::tEmail& a_email);
void MakeFile_mxschedulesetup();

int main(int argc, char* argv[])
{
	try
	{
		const std::string AppName = utils::GetAppNameMain(argv[0]);

		std::string PathFileConfig = utils::linux::GetPathConfigExc(AppName);
		std::string PathFilePrivate = utils::linux::GetPathConfigExc("mxprivate");

		dev::tDataSetConfig DsConfig(PathFileConfig, PathFilePrivate);

		dev::config::tEmail Email = DsConfig.GetEmail();

		dev::config::tConfigFiles ConfigFiles = DsConfig.GetConfigFiles();

		MakeFile_msmtprc(ConfigFiles.msmtprc, Email);

		std::string HostName = utils::linux::CmdLine("hostname");
		MakeFile_muttrc(ConfigFiles.muttrc, HostName, Email);

		if (MakeFile_fstab())
		{
			MakeFile_mxschedulesetup();

			utils::linux::CmdLine("reboot");
			return static_cast<int>(utils::tExitCode::EX_OK);
		}

		utils::linux::CmdLine("chmod 544 setup.sh");
		utils::linux::CmdLine("./setup.sh");
	}
	catch (std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
