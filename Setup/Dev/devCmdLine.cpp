#include "devCmdLine.h"

#include <utilsBase.h>

#include <cstring>
#include <iterator>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace dev
{

tCmdLine::tCmdLine(int argc, const char* argv[])
{
	if (argc < 2)
		THROW_INVALID_ARGUMENT("Wrong Args");

	for (auto& i : CmdList)
	{
		if (!strcmp(argv[1], i.Value))
		{
			Cmd = i.Cmd;
			break;
		}
	}

	if (Cmd == tCmd::None)
		THROW_INVALID_ARGUMENT("Wrong Cmd");

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
			THROW_INVALID_ARGUMENT("Wrong CmdOption");
	}

	if (Cmd == tCmd::Uninstall)
	{
		std::copy(CmdOptSet.rbegin(), CmdOptSet.rend(), std::back_inserter(CmdOptions));
	}
	else
	{
		std::copy(CmdOptSet.begin(), CmdOptSet.end(), std::back_inserter(CmdOptions));
	}
}

std::string tCmdLine::ToString() const
{
	std::string CmdLine;

	for (auto& i : CmdList)
	{
		if (Cmd == i.Cmd)
		{
			CmdLine += i.Value;
			break;
		}
	}

	for (auto i : CmdOptions)
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

}
