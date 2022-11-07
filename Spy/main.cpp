#include <devConfig.h>

#include <utilsBase.h>
#include <utilsChrono.h>
#include <utilsLinux.h>
#include <utilsPath.h>
#include <utilsWeb.h>

#include <devDataSetConfig.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
//#include <stdexcept>
#include <thread>

#include <boost/property_tree/json_parser.hpp>

//#define MXSPY_TEST

std::string ParseGLO(const boost::property_tree::ptree& pTree);
std::string ParseGPS(const boost::property_tree::ptree& pTree);

void GetGNSSState(const std::string& host, const std::string& target, const share_config::tOutFile& cfgOut, std::function<std::string(const boost::property_tree::ptree&)> handler)
{
#if defined(MXSPY_TEST)
	//std::fstream DataRaw = std::fstream("DataRaw_gps.txt", std::ios::in);
	std::fstream DataRaw = std::fstream("DataRaw_glonass.txt", std::ios::in);
	if (!DataRaw.is_open())
		return;//[TBD] throw an exception
#else
	std::optional<std::string> DataRawOpt = utils::web::HttpsReqSyncString(host, target);
	if (!DataRawOpt.has_value())
		return;//[TBD] throw an exception

	std::stringstream DataRaw(DataRawOpt.value());
#endif

	std::string DTStr = utils::GetDateTime();
	std::string Path = cfgOut.Path + "/";
	std::string FileName = cfgOut.Prefix + DTStr + ".json";
	std::string FileNameFull = Path + FileName;
	std::string FileNameTemp = Path + g_FileNameTempPrefix + FileName + ".tmp";
	std::fstream File = std::fstream(FileNameTemp, std::ios::out);
	if (!File.good())
		return;//[TBD] throw an exception

	std::string DataJSON;
	try
	{
		boost::property_tree::ptree PTree;
		boost::property_tree::json_parser::read_json(DataRaw, PTree);

		for (auto& item : PTree.get_child(""))
		{
			std::string Str = item.second.get<std::string>("name");
			if (Str != "OG")
				continue;

			auto Sats = item.second.get_child("data");
			DataJSON = handler(Sats);
			break;
		}
	}
	catch (std::exception const& e)
	{
		std::cerr << "[ERR] " << e.what() << std::endl;
	}

	File << "{\n";
	File << DataJSON;
	File << "\n}";
	File.close();

	if (DataJSON.empty())
	{
		std::remove(FileNameTemp.c_str());
	}
	else
	{
		std::rename(FileNameTemp.c_str(), FileNameFull.c_str());
	}

	utils::RemoveFilesOutdated(cfgOut.Path, cfgOut.Prefix, cfgOut.QtyMax);

#if defined(MXSPY_TEST)
	DataRaw.close();
#endif
}

int main(int argc, char* argv[])
{
	try
	{
		const std::filesystem::path Path{ argv[0] };
		std::filesystem::path PathFile = Path.filename();
		if (PathFile.has_extension())
			PathFile.replace_extension();

		std::string PathFileConfig = utils::linux::GetPathConfigExc(PathFile.string());
		std::string PathFileDevice = utils::linux::GetPathConfigExc("mxdevice");

		utils::tTimePeriod TimePeriod(true);

		while (true)
		{
			dev::tDataSetConfig DsConfig(PathFileConfig, PathFileDevice);

			dev::config::tSpyGNSS SpyGNSS = DsConfig.GetSpyGNSS();

			if (SpyGNSS.IsWrong())
				return static_cast<int>(utils::tExitCode::EX_CONFIG);

			TimePeriod.Set(SpyGNSS.Period);

			if (!TimePeriod.IsReady())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(60000));
				continue;
			}

			GetGNSSState(SpyGNSS.Host, SpyGNSS.TargetGLO, DsConfig.GetSpyOutGLO(), ParseGLO);
			GetGNSSState(SpyGNSS.Host, SpyGNSS.TargetGPS, DsConfig.GetSpyOutGPS(), ParseGPS);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}

