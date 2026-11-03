#include "dev/config.h"
#include "dev/dallas.h"

#include <shareUtilsFile.h>

#include <utilsExits.h>

#include <iostream>

int main(int argc, char* argv[])
{
	try
	{
		const dev::tConfig config(argv[0]);
		share::RemoveFilesOutdated(config.GetOutFile());

		dev::ThreadDALLAS(config);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return utils::exit_code::EX_IOERR;
	}

	return utils::exit_code::EX_OK;
}
