#include <devConfig.h>

#include <utilsBase.h>
#include <utilsChrono.h>
#include <utilsFile.h>
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

static std::string GetDateTimeNowString()
{
	std::time_t TimeNowRaw = std::time(nullptr);
	tm TimeNow = *std::gmtime(&TimeNowRaw);//gmtime may not be thread-safe.
	std::stringstream Stream;
	Stream << std::put_time(&TimeNow, "%F %T");
	return Stream.str();
}

std::string ParseGLO(const boost::property_tree::ptree& pTree);
std::string ParseGPS(const boost::property_tree::ptree& pTree);

bool GetGNSSState(const std::string& host, const std::string& target, const share_config::tOutFile& cfgOut, std::function<std::string(const boost::property_tree::ptree&)> handler)
{
#if defined(MXSPY_TEST)
	//std::fstream DataRaw = std::fstream("DataRaw_gps.txt", std::ios::in);
	std::fstream DataRaw = std::fstream("DataRaw_glonass.txt", std::ios::in);
	if (!DataRaw.is_open())
		return false;
#else
	std::optional<std::string> DataRawOpt = utils::web::HttpsReqSyncString(host, target);
	if (!DataRawOpt.has_value())
		return false;

	std::stringstream DataRaw(DataRawOpt.value());
#endif

	std::string DTStr = utils::GetDateTime();
	std::string Path = cfgOut.Path + "/";
	std::string FileName = cfgOut.Prefix + DTStr + ".json";
	std::string FileNameFull = Path + FileName;
	std::string FileNameTemp = Path + g_FileNameTempPrefix + FileName + ".tmp";
	std::fstream File = std::fstream(FileNameTemp, std::ios::out);
	if (!File.good())
		return true;//true means - do not repeate [TBD] throw an exception

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
	File << "\"date_time\": \"" << GetDateTimeNowString() << "\",\n";
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

	utils::file::RemoveFilesOutdated(cfgOut.Path, cfgOut.Prefix, cfgOut.QtyMax);

#if defined(MXSPY_TEST)
	DataRaw.close();
#endif

	return true;
}

int main(int argc, char* argv[])
{
	try
	{
		const std::string AppName = utils::GetAppNameMain(argv[0]);

		std::string PathFileConfig = utils::linux::GetPathConfigExc(AppName);
		std::string PathFileDevice = utils::linux::GetPathConfigExc("mxdevice");

		utils::tTimePeriodCount TimePeriod(true);

		while (true)
		{
			dev::tDataSetConfig DsConfig(PathFileConfig, PathFileDevice);
			dev::config::tSpyGNSS SpyGNSS = DsConfig.GetSpyGNSS();
			if (SpyGNSS.IsWrong())
				return static_cast<int>(utils::tExitCode::EX_CONFIG);

			if (TimePeriod.GetPeriod() != SpyGNSS.Period || TimePeriod.GetRepPeriod() != SpyGNSS.RepPeriod || TimePeriod.GetRepQty() != SpyGNSS.RepQty)
				TimePeriod.Set(SpyGNSS.Period, SpyGNSS.RepPeriod, SpyGNSS.RepQty, false);

			if (!TimePeriod.IsReady())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				continue;
			}

			if (!GetGNSSState(SpyGNSS.Host, SpyGNSS.TargetGLO, DsConfig.GetSpyOutGLO(), ParseGLO))
				continue;

			if (!GetGNSSState(SpyGNSS.Host, SpyGNSS.TargetGPS, DsConfig.GetSpyOutGPS(), ParseGPS))
				continue;

			TimePeriod.Complete();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "[ERR] " << e.what() << "\n";

		return static_cast<int>(utils::tExitCode::EX_NOINPUT);
	}

	return static_cast<int>(utils::tExitCode::EX_OK);
}

