#pragma once

#include <string>
#include <vector>

namespace dev
{

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
	Download,
	Fstab,
	Email,
	Script,

	Final // It's the last state (it can remove its temporary directories, files and the like)
};

struct tCmdLine
{
	tCmd Cmd = tCmd::None;
	std::vector<tCmdOption> CmdOptions;

	tCmdLine() = delete;
	tCmdLine(int argc, const char* argv[]);

	std::string ToString() const;
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
	{ tCmdOption::Download, "-download" },
	{ tCmdOption::Fstab, "-fstab" },
	{ tCmdOption::Email, "-email" },
	{ tCmdOption::Script, "-script" },
	{ tCmdOption::Final, "-final" },
};

}
