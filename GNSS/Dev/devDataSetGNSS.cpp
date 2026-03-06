#include "devDataSetGNSS.h"

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
		SStr << ",\n \"mode_indicator\": \"" << Mode << "\"";
	if (Latitude.has_value() && Longitude.has_value())
	{
		SStr.precision(6);
		SStr << ",\n \"latitude\": " << *Latitude;
		SStr << ",\n \"longitude\": " << *Longitude;
	}
	if (Altitude.has_value())
		SStr << ",\n \"altitude\": " << *Altitude;
	SStr.precision(2);
	if (Speed.has_value())
		SStr << ",\n \"speed\": " << *Speed;
	if (Course.has_value())
		SStr << ",\n \"course\": " << *Course;

	if (!Satellites.empty())
	{
		SStr << ",\n \"satellites\": [\n";
		for (std::size_t i = 0; i < Satellites.size(); ++i)
		{
			SStr << "  {\n";
			SStr << "   \"id\": " << static_cast<int>(Satellites[i].ID) << ",\n";
			SStr << "   \"elevation\": " << static_cast<int>(Satellites[i].Elevation) << ",\n";
			SStr << "   \"azimuth\": " << static_cast<int>(Satellites[i].Azimuth) << ",\n";
			SStr << "   \"snr\": " << static_cast<int>(Satellites[i].SNR) << "\n";
			SStr << "  }";
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
		SStr << ",\n \"hw_model\": \"" << ReceiverModel << "\"";
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
	auto sdgfs = std::gmtime(&time);
	if (std::strftime(TempStr, sizeof(TempStr), "%F %T", std::gmtime(&time)))
		return TempStr;
	return {};
}

}
