#include <utilsBase.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <devDataSet.h>

#include <fstream>
#include <iostream>

#include <boost/filesystem.hpp>

namespace dev
{

void MakeFile_msmtprc(const std::string& a_path, const config::tEmail& a_email);
void MakeFile_muttrc(const std::string& a_path, const std::string& a_hostname, const config::tEmail& a_email);

}

int main(int argc, char* argv[])
{
	try
	{
		const boost::filesystem::path Path{ argv[0] };
		boost::filesystem::path PathFile = Path.filename();
		if (PathFile.has_extension())
			PathFile.replace_extension();

		std::string FileNameConf = utils::linux::GetPathConfig(PathFile.string());

		dev::tDataSetConfig DsConfig(FileNameConf);

		dev::config::tEmail Email = DsConfig.GetEmail();

		dev::config::tConfigFiles ConfigFiles = DsConfig.GetConfigFiles();

		dev::MakeFile_msmtprc(ConfigFiles.msmtprc, Email);

		std::string HostName = utils::linux::CmdLine("hostname");
		dev::MakeFile_muttrc(ConfigFiles.muttrc, HostName, Email);

		//[TBD] it's possible to make files in one folder and then move them with script into appropriate folders and set them access permissions
		//[TBD] set access permissions to the files and maybe reboot the system - maybe with bash-script
	}
	catch (std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
