#include <devConfig.h>

#include <devDataSetConfig.h>
#include <devStateInit.h>
#include <devStateReceive.h>

//#include <utilsException.h>
#include <utilsExits.h>
#include <utilsPath.h>

#include <thread>
#include <iostream> // [TEST]

#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
	const std::filesystem::path Path(argv[0]);
	const std::string AppName = utils::path::GetAppNameMain(Path);
	const std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
	const std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();

	dev::tDataSetConfig DsConfig = dev::tDataSetConfig(PathFileConfig, PathFileMX);
	if (DsConfig.GetUART().IsWrong())
		return static_cast<int>(utils::exit_code::EX_CONFIG);
	//std::shared_ptr<dev::tDataSetConfig> DsConfig = std::make_shared<dev::tDataSetConfig>(PathFileConfig, PathFileMX);
	//if (DsConfig->GetUART().IsWrong())
	//	return static_cast<int>(utils::exit_code::EX_CONFIG);
		//THROW_RUNTIME_ERROR("UART is not valid.");

	boost::asio::io_context ioc;

	const std::string PortID = DsConfig.GetUART().ID;
	dev::tPortUART Port(ioc, PortID, DsConfig.GetUART().BR);

	std::thread Thread_ioc([&ioc]() { ioc.run(); });

	auto [DsHW, Status] = dev::state::Init(Port, DsConfig);
	
	std::cout << DsHW.ToString() << '\n'; // [TBD] write into log

	if (Status != dev::state::tStatus::None)
	{
		// [TBD] Handle the error!
	}

	Status = dev::state::Receive(Port, DsHW);
	if (Status != dev::state::tStatus::None)
	{
		// [TBD] Handle the error!
	}
	
	// [TBD] Error Handling or exit

	ioc.stop();
	Thread_ioc.join();

	return static_cast<int>(utils::exit_code::EX_OK);
}
