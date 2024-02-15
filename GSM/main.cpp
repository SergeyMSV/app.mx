#include <devConfig.h>

#include <devDataSetConfig.h>
//#include <devGNSS.h>
//#include <devLog.h>
//#include <devShell.h>

#include <utilsBase.h>
//#include <utilsFile.h>
#include <utilsPath.h>
#include <utilsPortSerial.h>

//#include <shareUtilsFile.h>
//
//#include <atomic>
//#include <chrono>
//#include <filesystem>
//#include <functional>
//#include <future>
#include <iostream>
//#include <mutex>

#include <boost/asio.hpp>

class tBoard : public utils::port_serial::tPortSerialAsync<>
{
	std::vector<std::uint8_t> m_DataReceived;
	mutable std::mutex m_Mtx;
public:
	tBoard(boost::asio::io_context& io, const std::string& id)
		:tPortSerialAsync(io, id, 115200, utils::port_serial::tCharSize(8), utils::port_serial::tStopBits::one, utils::port_serial::tParity::none, utils::port_serial::tFlowControl::hardware)
	{
	}
	virtual ~tBoard()
	{
	}

	bool IsReceived() const { return !m_DataReceived.empty(); }
	std::vector<std::uint8_t> GetReceived()
	{
		std::lock_guard<std::mutex> Lock(m_Mtx);
		std::vector<std::uint8_t> Data = std::move(m_DataReceived);
		return Data;
	}

protected:
	void OnReceived(std::vector<std::uint8_t>& data) override
	{
		std::lock_guard<std::mutex> Lock(m_Mtx);
		m_DataReceived.insert(m_DataReceived.end(), data.begin(), data.end());
	}
};

bool Ñontains(const std::string& str, const std::string& subs)
{
	return str.find(subs) != std::string::npos;
}

void main_cinterion_bgs2_email_send(tBoard& port)
{
	std::string Received;
	bool Rsp = false;
	for (int i= 0; i < 100; ++i)
	{
		if (port.IsReceived())
		{
			std::vector<std::uint8_t> Data = port.GetReceived();
			std::string Text(Data.begin(), Data.end());
			Received += Text;
		}

		if (Ñontains(Received, "+CREG: 1")) // [TBD] it's possible to get Pos and remove decoded data and then handle the rest of the string
		{
			Rsp = true;
			break;
		}
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path(argv[0]);
		const std::string AppName = utils::path::GetAppNameMain(Path);
		std::string PathFileConfig = utils::path::GetPathConfigExc(AppName).string();
		std::string PathFileMX = utils::path::GetPathConfigExc("mx").string();
		std::string PathFilePrivate = utils::path::GetPathConfigExc("mxprivate").string();

		dev::tDataSetConfig DsConfig(PathFileConfig, PathFileMX);

		//dev::g_Settings = dev::tDataSetConfig(PathFileConfig, PathFilePrivate);

		boost::asio::io_context ioc;

		const std::string PortID = "COM1";

		tBoard Port(ioc, PortID);

		std::thread Thread_IO([&ioc]() { ioc.run(); });

		for (int i = 0; i < 2; ++i)
		{
			main_cinterion_bgs2_email_send(Port);

			std::this_thread::sleep_for(std::chrono::seconds(10)); // [#]
		}

	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return static_cast<int>(utils::tExitCode::EX_IOERR);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}
