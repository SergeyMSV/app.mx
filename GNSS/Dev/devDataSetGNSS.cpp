#include "devDataSetGNSS.h"

#include <utilsTime.h>

#include <shareUtilsFile.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace dev
{

std::string tDataSetGNSS::ToString() const
{
	std::stringstream SStr;
	SStr << "DT: " << ToString(DateTime.value_or({})) << ", Lat: " << Latitude.value_or(0) << ", Lon: " << Longitude.value_or(0) << ", Spd: " << Speed.value_or(0) << ", Crs: " << Course.value_or(0);
	SStr << '\n' << ToJSON(); // [TEST]
	return SStr.str();
}

std::string tDataSetGNSS::ToJSON() const
{
	std::stringstream SStr;
	SStr.setf(std::ios::fixed);
	SStr.fill('0');
	SStr << "{\n";
	SStr << " \"utc\": \"" << ToString(DateTime.value_or({})) << "\"";
	//SStr << ",\n \"utc_raw\": \"" << ToStringOutdated(DateTime.value_or({})) << "\",\n";
	SStr << ",\n \"valid\": \"" << (Valid ? 'A' : 'V') << "\"";
	if (!Mode.empty())
		SStr << ",\n \"mode\": \"" << Mode << "\""; // mode_indicator
	if (Latitude.has_value() && Longitude.has_value())
	{
		SStr.precision(6);
		SStr << ",\n \"lat\": " << *Latitude; // latitude
		SStr << ",\n \"lon\": " << *Longitude; // longitude
	}
	if (Altitude.has_value())
	{
		SStr.precision(3);
		SStr << ",\n \"msl\": " << *Altitude; // altitude (msl)
	}
	SStr.precision(2);
	if (Speed.has_value())
		SStr << ",\n \"spd\": " << *Speed; // speed
	if (Course.has_value())
		SStr << ",\n \"crs\": " << *Course; // course

	if (!Satellites.empty())
	{
		SStr << ",\n \"sats\": [\n"; // satellites
		for (std::size_t i = 0; i < Satellites.size(); ++i)
		{
			SStr << "  {";
			SStr << "   \"id\": " << static_cast<int>(Satellites[i].ID) << ",";
			SStr << "   \"elv\": " << static_cast<int>(Satellites[i].Elevation) << ","; // elevation
			SStr << "   \"azm\": " << static_cast<int>(Satellites[i].Azimuth) << ","; // azimuth
			SStr << "   \"snr\": " << static_cast<int>(Satellites[i].SNR);
			SStr << " }";
			if (i < Satellites.size() - 1)
				SStr << ",";
			SStr << "\n";
		}
		SStr << " ]";
	}

	/*SStr << ",\n \"jamming\": [\n";
	SStr.precision(6); // for jamming frequency
	std::size_t counter = 0;
	for (const auto& i : Jamming)
	{
		SStr << "  {\n";
		SStr << "   \"index\": " << i.first << ",\n";
		SStr << "   \"frequency\": " << i.second << "\n";
		SStr << "  }";
		if (++counter < Jamming.size())
		{
			SStr << ",";
		}
		SStr << "\n";
	}
	SStr << " ]";*/
	if (!ReceiverModel.empty())
		SStr << ",\n \"model\": \"" << ReceiverModel << "\"";
	SStr << "\n}";
	return SStr.str();
}

std::string tDataSetGNSS::ToString(tDateTime time) const
{
	std::stringstream SStr;
	SStr << ToString(time.Seconds) << ".";
	SStr << std::setw(3) << std::setfill('0') << std::to_string(time.Milliseconds);
	return SStr.str();
}

std::string tDataSetGNSS::ToString(std::time_t time) const
{
	char TempStr[30];
	if (std::strftime(TempStr, sizeof(TempStr), "%F %T", std::gmtime(&time)))
		return TempStr;
	return {};
}

void tDataSetGNSS::ToFile(const share::config::tOutFile& configOut) const
{
	const std::string DTStr = utils::time::tDateTime::Now().ToStringPath();

	const std::filesystem::path Path = configOut.Path;
	const std::string FileName = configOut.Prefix + DTStr + ".json";
	const std::string TempFileName = settings::FileNameTempPrefix + FileName + ".tmp";

	std::filesystem::path TempFilePath = Path / TempFileName;
	std::fstream File = std::fstream(TempFilePath, std::ios::out);
	if (!File.good())
		return; // [TBD] may be an exception should be throwned
	File << ToJSON();
	File.close();

	std::filesystem::path FilePath = Path / FileName;
	std::error_code ec;
	std::filesystem::rename(TempFilePath, FilePath, ec);
	share::RemoveFilesOutdated(configOut);
}

}
