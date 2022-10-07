#include "devDataSetGNSS.h"

#include <sstream>

#include <boost/property_tree/json_parser.hpp>

namespace dev
{

tDataSetGNSS::tSatellite::tSatellite(boost::property_tree::ptree pTree)
{
	const uint8_t IDRaw = pTree.get<uint8_t>("id");

	GNSS = GetGNSSCode(IDRaw);
	ID = GetGNSSSatelliteID(IDRaw);

	Elevation = pTree.get<uint8_t>("elevation");
	Azimuth = pTree.get<uint16_t>("azimuth");
	SNR = pTree.get<uint8_t>("snr");
}

utils::tGNSSCode tDataSetGNSS::tSatellite::GetGNSSCode(uint8_t satIDRaw) const
{
	if (satIDRaw >= 1 && satIDRaw <= 32)
		return utils::tGNSSCode::GPS;

	if (satIDRaw >= 33 && satIDRaw <= 64)
		return utils::tGNSSCode::WAAS;

	if (satIDRaw >= 65 && satIDRaw <= 96)
		return utils::tGNSSCode::GLONASS;

	return utils::tGNSSCode::Unknown;
}

uint8_t tDataSetGNSS::tSatellite::GetGNSSSatelliteID(uint8_t satIDRaw) const
{
	utils::tGNSSCode Code = GetGNSSCode(satIDRaw);

	switch (Code)
	{
	case utils::tGNSSCode::GPS: return satIDRaw;
	case utils::tGNSSCode::WAAS: return satIDRaw + 87;//see protocol NMEA
	case utils::tGNSSCode::GLONASS: return satIDRaw - 64;//see protocol NMEA
	}

	return 0xFF;
}

tDataSetGNSS::tDataSetGNSS(const std::string& a_fileName)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(a_fileName, PTree);

	UTC = PTree.get<std::string>("utc");
	Valid = PTree.get<bool>("valid");
	Latitude = PTree.get<double>("latitude");
	Longitude = PTree.get<double>("longitude");
	Altitude = PTree.get<double>("altitude");
	Speed = PTree.get<double>("speed");
	Course = PTree.get<double>("course");

	for (auto& sat : PTree.get_child("satellite"))
	{
		Satellites.emplace_back(sat.second);
	}
}

std::string tDataSetGNSS::GetHTMLTable(const std::string& styleCol1) const
{
	std::stringstream Table;
	Table << "<table>";

	Table << "<tr><td><b>GNSS</b></td></tr>";
	Table << "<tr><td>";
	Table << "<table>";	
	Table << "<tr><td " + styleCol1 + ">UTC</td><td>" + UTC + "</td></tr>";
	Table << "<tr><td>Valid</td><td>";
	Table << Valid ? "A" : "V";
	Table << "</td></tr>";

	Table << std::fixed;
	Table.precision(6);
	Table << "<tr><td>Latitude</td><td>" << Latitude << "</td></tr>";
	Table << "<tr><td>Longitude</td><td>" << Longitude << "</td></tr>";
	Table << "<tr><td>Altitude</td><td>" << Altitude << "</td></tr>";
	Table.precision(2);
	Table << "<tr><td>Speed</td><td>" << Speed << " kn (" << Speed * 1.852 << " km/h)</td></tr>";
	Table << "<tr><td>Course</td><td>" << Course << "</td></tr>";
	Table << "</table>";

	Table << "</td></tr>";

	Table << "<tr><td><b>GPS</b></td></tr>";
	Table << "<tr><td>";
	Table << GetHTMLTableSatellitesVert(utils::tGNSSCode::GPS);
	Table << "</td></tr>";

	Table << "<tr><td><b>GLONASS</b></td></tr>";
	Table << "<tr><td>";
	Table << GetHTMLTableSatellitesVert(utils::tGNSSCode::GLONASS);
	Table << "</td></tr>";

	Table << "<tr><td><b>WAAS</b></td></tr>";
	Table << "<tr><td>";
	Table << GetHTMLTableSatellitesVert(utils::tGNSSCode::WAAS);
	Table << "</td></tr>";

	Table << "</td></tr></table>";

	return Table.str();
}

std::string tDataSetGNSS::GetHTMLTableSatellitesHor(utils::tGNSSCode codeGNSS) const
{
	std::string Table;
	Table += "<table>";

	std::string StrID = codeGNSS == utils::tGNSSCode::GPS || codeGNSS == utils::tGNSSCode::WAAS ? "PRN" : "ID";

	std::string Row1 = "<tr><td>" + StrID + "</td>";
	std::string Row2 = "<tr><td>Elevation</td>";
	std::string Row3 = "<tr><td>Azimuth</td>";
	std::string Row4 = "<tr><td>SNR</td>";

	for (auto& i : Satellites)
	{
		if (i.GNSS != codeGNSS)
			continue;

		Row1 += "<td>" + std::to_string(i.ID) + "</td>";
		Row2 += "<td>" + std::to_string(i.Elevation) + "</td>";
		Row3 += "<td>" + std::to_string(i.Azimuth) + "</td>";
		Row4 += "<td>" + std::to_string(i.SNR) + "</td>";
	}

	Row1 += "</tr>";
	Row2 += "</tr>";
	Row3 += "</tr>";
	Row4 += "</tr>";

	Table += Row1;
	Table += Row2;
	Table += Row3;
	Table += Row4;

	Table += "</table>";

	return Table;
}

std::string tDataSetGNSS::GetHTMLTableSatellitesVert(utils::tGNSSCode codeGNSS) const
{
	std::string Table;
	Table += "<table>";

	std::string StrID = codeGNSS == utils::tGNSSCode::GPS || codeGNSS == utils::tGNSSCode::WAAS ? "PRN" : "ID";

	Table += "<tr align = \"center\"><td width=\"50\">" + StrID + "</td><td>Elevation</td><td>Azimuth</td><td>SNR</td></tr>";

	for (auto& i : Satellites)
	{
		if (i.GNSS != codeGNSS)
			continue;

		Table += "<tr align = \"center\"><td>" + std::to_string(i.ID) + "</td><td>" + std::to_string(i.Elevation) + "</td><td>" + std::to_string(i.Azimuth) + "</td><td>" + std::to_string(i.SNR) + "</td></tr>";
	}

	Table += "</table>";

	return Table;
}

}
