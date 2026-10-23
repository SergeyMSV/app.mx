#include "dev/config.h"
#include "dev/dallas.h"

#include <utilsExits.h>

#include <iostream>

int main(int argc, char* argv[])
{
	try
	{
		const dev::tConfig DsConfig;
		dev::ThreadDALLAS(DsConfig);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::exit_code::EX_IOERR);
	}

	return static_cast<int>(utils::exit_code::EX_OK);
}
