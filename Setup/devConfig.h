#pragma once

#include <string>
#include <vector>

enum class tCmd
{
	None,
	Install,
	Uninstall,
};

enum class tCmdOption
{
	None,// Install everything
	// CmdOptions are executed in the same order
	Fstab,
	Email,
	Script,
};

struct tCmdLine
{
	tCmd Cmd = tCmd::None;
	std::vector<tCmdOption> CmdOptions;

	tCmdLine() = default;
};

struct tCmdListItem
{
	tCmd Cmd;
	const char* Value;
};

constexpr tCmdListItem CmdList[] =
{
	{ tCmd::None, "" },
	{ tCmd::Install, "install" },
	{ tCmd::Uninstall, "uninstall" },	
};

struct tCmdOptionListItem
{
	tCmdOption CmdOption;
	const char* Value;
};

constexpr tCmdOptionListItem CmdOptionList[] =
{
	{ tCmdOption::None, "" },
	{ tCmdOption::Fstab, "-fstab" },
	{ tCmdOption::Email, "-email" },
	{ tCmdOption::Script, "-script" },
};

struct tAppData
{
	std::string AppName;
	std::string WorkingDirectory;
	std::string Path;
};
