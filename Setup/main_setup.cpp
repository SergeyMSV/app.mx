#include <utilsLinux.h>

#include <devConfig.h>
#include <devCmdLine.h>
#include <devDataSetConfig.h>

#include <shareWeb.h>

#include <filesystem>
#include <string>
#include <vector>

#include <7z2201/utilsArch_7z2201_simple.h>
#include <openssl/sha.h>

void MakeFile_msmtprc(const std::string& a_path, const dev::config::tEmail& a_email);
void MakeFile_muttrc(const std::string& a_path, const std::string& a_hostname, const dev::config::tEmail& a_email);

bool SetupFstab(const dev::config::tFstab& fstabConf);
bool RevertFstab(const dev::config::tFstab& fstabConf);

void Install_Email(const dev::tDataSetConfig& dsConfig)
{
	dev::config::tConfigFiles ConfigFiles = dsConfig.GetConfigFiles();
	dev::config::tEmail Email = dsConfig.GetEmail();

	MakeFile_msmtprc(ConfigFiles.msmtprc, Email);

	std::string HostName = utils::linux::CmdLine("hostname");
	MakeFile_muttrc(ConfigFiles.muttrc, HostName, Email);
}

void ScriptStart(const tAppData& appData, const std::string& cmd)
{
	std::string ScrName = appData.AppName + ".sh";
	std::string CmdCDPathWorkDir = "cd " + appData.WorkingDirectory + "; ";
	utils::linux::CmdLine(CmdCDPathWorkDir + "chmod 544 " + ScrName);
	utils::linux::CmdLine(CmdCDPathWorkDir + "./" + ScrName + " " + cmd);
}

bool Install(dev::tCmdLine& cmdLine, const dev::tDataSetConfig& dsConfig, const tAppData& appData)
{
	bool Reboot = false;
	for (size_t i = 0; i < cmdLine.CmdOptions.size(); ++i)
	{
		switch (cmdLine.CmdOptions[i])
		{
		case dev::tCmdOption::None:
			return false;
		case dev::tCmdOption::Download:
		{
			std::string UpdateFileZip = GetUpdateFile(dsConfig.GetUpdateServer(), dsConfig.GetDevice(), dsConfig.GetUpdatePath());
			if (UpdateFileZip.empty())
				return static_cast<int>(utils::tExitCode::EX_OK);

			utils::arch_7z2201_simple::Decompress(UpdateFileZip);

			std::filesystem::remove(UpdateFileZip);

			// mxsetup from downloaded folder shall be called with the same options except -download
			cmdLine.CmdOptions = { cmdLine.CmdOptions.begin() + i + 1, cmdLine.CmdOptions.end() };

			std::string CmdCDPathMX = "cd " + dsConfig.GetUpdatePath() + "; ";
			utils::linux::CmdLine(CmdCDPathMX + "chmod 544 mxsetup");
			utils::linux::CmdLine(CmdCDPathMX + "./mxsetup " + cmdLine.ToString());
			return false;
		}
		case dev::tCmdOption::Fstab:
		{
			if (SetupFstab(dsConfig.GetFstab()))
				Reboot = true;
			break;
		}
		case dev::tCmdOption::Email:
		{
			dev::config::tConfigFiles ConfigFiles = dsConfig.GetConfigFiles();
			dev::config::tEmail Email = dsConfig.GetEmail();
			std::string HostName = utils::linux::CmdLine("hostname");
			MakeFile_msmtprc(ConfigFiles.msmtprc, Email);
			MakeFile_muttrc(ConfigFiles.muttrc, HostName, Email);
			break;
		}
		case dev::tCmdOption::Script:
		{
			ScriptStart(appData, "install");
			break;
		}
		}

		if (Reboot)
		{
			cmdLine.CmdOptions = { cmdLine.CmdOptions.begin() + i + 1, cmdLine.CmdOptions.end() };
			break;
		}
	}
	return Reboot;
}

bool Uninstall(dev::tCmdLine& cmdLine, const dev::tDataSetConfig& dsConfig, const tAppData& appData)
{
	bool Reboot = false;
	for (size_t i = 0; i < cmdLine.CmdOptions.size(); ++i)
	{
		switch (cmdLine.CmdOptions[i])
		{
		case dev::tCmdOption::None:
			return false;
		case dev::tCmdOption::Fstab:
		{
			if (RevertFstab(dsConfig.GetFstab()))
				Reboot = true;
			break;
		}
		case dev::tCmdOption::Email:
		{
			dev::config::tConfigFiles ConfigFiles = dsConfig.GetConfigFiles();
			std::filesystem::remove(ConfigFiles.msmtprc);
			std::filesystem::remove(ConfigFiles.muttrc);
			break;
		}
		case dev::tCmdOption::Script:
		{
			ScriptStart(appData, "uninstall");
			break;
		}
		}

		if (Reboot)
		{
			cmdLine.CmdOptions = { cmdLine.CmdOptions.begin() + i + 1, cmdLine.CmdOptions.end() };
			break;
		}
	}
	return Reboot;
}

bool Setup(dev::tCmdLine& cmdLine, const dev::tDataSetConfig& dsConfig, const tAppData& appData)
{
	switch (cmdLine.Cmd)
	{
	case dev::tCmd::None:
		return false;
	case dev::tCmd::Install:
	{
		if (Install(cmdLine, dsConfig, appData))
			return true;
		break;
	}
	case dev::tCmd::Uninstall:
	{
		if (Uninstall(cmdLine, dsConfig, appData))
			return true;
		break;
	}
	}

	return false;
}
