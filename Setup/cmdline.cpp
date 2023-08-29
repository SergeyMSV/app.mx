#include <devConfig.h>
#include <devDataSetConfig.h>

#include <set>
#include <string>
#include <vector>

tCmdLine CmdLine_Parse(int argc, const char* argv[])
{
	if (argc < 2)
		return {};

	tCmdLine CmdLine;

	for (auto& i : CmdList)
	{
		if (!strcmp(argv[1], i.Value))
		{
			CmdLine.Cmd = i.Cmd;
			break;
		}
	}

	if (CmdLine.Cmd == tCmd::None)
		throw std::invalid_argument("Wrong Cmd");

	std::set<tCmdOption> CmdOptSet;

	for (int i = 2; i < argc; ++i)
	{
		const size_t QtyFound = CmdOptSet.size();

		for (auto& opt : CmdOptionList)
		{
			if (!strcmp(argv[i], opt.Value))
			{
				CmdOptSet.insert(opt.CmdOption);
				break;
			}
		}

		if (QtyFound == CmdOptSet.size())
			throw std::invalid_argument("Wrong CmdOption");
	}

	if (CmdLine.Cmd == tCmd::Uninstall)
	{
		std::copy(CmdOptSet.rbegin(), CmdOptSet.rend(), std::back_inserter(CmdLine.CmdOptions));
	}
	else
	{
		std::copy(CmdOptSet.begin(), CmdOptSet.end(), std::back_inserter(CmdLine.CmdOptions));
	}

	return CmdLine;
}

std::string CmdLine_Make(tCmdLine cmdLine)
{
	std::string CmdLine;

	for (auto& i : CmdList)
	{
		if (cmdLine.Cmd == i.Cmd)
		{
			CmdLine += i.Value;
			break;
		}
	}

	for (auto i : cmdLine.CmdOptions)
	{
		for (auto& opt : CmdOptionList)
		{
			if (i == opt.CmdOption)
			{
				CmdLine += " ";
				CmdLine += opt.Value;
				break;
			}
		}
	}

	return CmdLine;
}
