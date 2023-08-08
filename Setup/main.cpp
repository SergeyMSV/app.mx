#include <utilsBase.h>
#include <utilsLinux.h>
#include <utilsPath.h>

#include <devConfig.h>
#include <devCmdLine.h>
#include <devDataSetConfig.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

//#define TEST_SETUP

bool Setup(dev::tCmdLine& cmdLine, const dev::tDataSetConfig& dsConfig, const tAppData& appData);

void MakeMXSetupSchedule(const std::string& appPath, const std::string& appArg);
void RemoveMXSetupSchedule();

// mxsetup install (-fstab -email -script)              working directory contains the called mxsetup,  OK
// mxsetup install -download (-fstab -email -script)    working directory contains the called mxsetup,  OK
// mxsetup uninstall (-fstab -email -script)            working directory contains the called mxsetup,  OK
// mxsetup uninstall -download ...                                                                      WRONG

#ifdef TEST_SETUP
void TestCmdLine();
#endif // TEST_SETUP

int main(int argc, const char* argv[])
{
#ifdef TEST_SETUP
	TestCmdLine();
#endif // TEST_SETUP

	try
	{
		RemoveMXSetupSchedule();

		dev::tCmdLine CmdLine(argc, argv);

		if (CmdLine.Cmd == dev::tCmd::None || CmdLine.CmdOptions.empty())
			return static_cast<int>(utils::tExitCode::EX_OK);

		const std::string AppName = utils::GetAppNameMain(argv[0]);

		tAppData AppData;

		AppData.Path = argv[0];
		const std::filesystem::path Path{ argv[0] };
		std::filesystem::path WorkDir = Path.parent_path();
		AppData.WorkingDirectory = WorkDir.string();
		AppData.AppName = AppName;

		std::string PathFileConfig = utils::linux::GetPathConfigExc(AppData.AppName);
		std::string PathFileDevice = utils::linux::GetPathConfigExc("mxdevice");
		std::string PathFilePrivate = utils::linux::GetPathConfigExc("mxprivate");
		dev::tDataSetConfig DsConfig(PathFileConfig, PathFileDevice, PathFilePrivate);

		if (Setup(CmdLine, DsConfig, AppData))
		{
			MakeMXSetupSchedule(AppData.Path, CmdLine.ToString());

			utils::linux::CmdLine("reboot");
			return static_cast<int>(utils::tExitCode::EX_OK);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}

#ifdef TEST_SETUP
void TestCmdLine()
{
	try
	{
		const char* Argv[] = { "mxsetup","install","-fstab","-script","-download","-email",0 };
		tCmdLine CmdLine = CmdLine_Parse(6, Argv);
		std::cerr << "OK\n";
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}

	try
	{
		const char* Argv[] = { "mxsetup","install","-fstab","-script","-smthelse","-email",0 };
		tCmdLine CmdLine = CmdLine_Parse(6, Argv);
	}
	catch (std::exception& e)
	{
		std::cerr << "ERR_OK: " << e.what() << "\n";
	}

	try
	{
		const char* Argv[] = { "mxsetup","install","-fstab","-script","-email","-email",0 };
		tCmdLine CmdLine = CmdLine_Parse(6, Argv);
	}
	catch (std::exception& e)
	{
		std::cerr << "ERR_OK: " << e.what() << "\n";
	}
}
#endif // TEST_SETUP