#include "devDataSetGNSS.h"

#include <shareGNSS.h>
#include <shareHTML.h>

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

tDataSetGNSS::tDataSetGNSS(const std::string& fileName)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(fileName, PTree);

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

void tDataSetGNSS::SetStateGLO(const std::string& fileName)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(fileName, PTree);

	for (auto& sat : PTree.get_child("glonass"))
	{
		share_gnss::tSatDesc SatDesc(sat.second);
		if (SatDesc.Status != 1)
			continue;

		auto FindSat = [&SatDesc](const tSatellite& value)
		{
			return
				value.GNSS == utils::tGNSSCode::GLONASS &&
				value.ID == SatDesc.Slot;
		};

		auto Iter = std::find_if(Satellites.begin(), Satellites.end(), FindSat);
		if (Iter == Satellites.end())
			continue;

		Iter->Type = SatDesc.TypeKA;
		Iter->COSMOS = SatDesc.COSMOS;
		Iter->NORAD = SatDesc.NORAD;
	}
}

void tDataSetGNSS::SetStateGPS(const std::string& fileName)
{
	boost::property_tree::ptree PTree;
	boost::property_tree::json_parser::read_json(fileName, PTree);

	for (auto& sat : PTree.get_child("gps"))
	{
		share_gnss::tSatDesc SatDesc(sat.second);

		auto FindSat = [&SatDesc](const tSatellite& value)
		{
			return
				value.GNSS == utils::tGNSSCode::GPS &&
				value.ID == SatDesc.PRN;
		};

		auto Iter = std::find_if(Satellites.begin(), Satellites.end(), FindSat);
		if (Iter == Satellites.end())
			continue;

		Iter->Type = SatDesc.TypeKA;
		Iter->NORAD = SatDesc.NORAD;
	}
}

std::string tDataSetGNSS::GetHTMLTable(const std::string& styleCol1) const
{
	std::string StyleHeader = share::GetHTMLBgColour(share::tHTMLFieldStatus::TableHeader);

	std::stringstream Table;

	Table << "<table>";
	Table << "<tr><td " << styleCol1 << "><b>GNSS</b></td></tr>";
	Table << "<tr><td " << StyleHeader << ">UTC</td><td>" << UTC << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Status</td><td>";
	Table << (Valid ? "+" : "-");
	Table << "</td></tr>";

	Table << std::fixed;
	Table.precision(6);
	Table << "<tr><td" << StyleHeader << ">Latitude</td><td>" << Latitude << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Longitude</td><td>" << Longitude << "</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Altitude</td><td>" << Altitude << "</td></tr>";
	Table.precision(2);
	Table << "<tr><td" << StyleHeader << ">Speed</td><td>" << Speed << " kn (" << Speed * 1.852 << " km/h)</td></tr>";
	Table << "<tr><td" << StyleHeader << ">Course</td><td>" << Course << "</td></tr>";
	Table << "</table>";

	Table << GetHTMLTableSatellitesVert(utils::tGNSSCode::GPS);
	Table << GetHTMLTableSatellitesVert(utils::tGNSSCode::GLONASS);
	Table << GetHTMLTableSatellitesVert(utils::tGNSSCode::WAAS);

	return Table.str();
}

std::string tDataSetGNSS::GetHTMLTableSatellitesHor(utils::tGNSSCode codeGNSS) const
{
	std::string StyleHeader = share::GetHTMLBgColour(share::tHTMLFieldStatus::TableHeader);

	std::string Table;
	Table += "<table>";

	std::string StrID = codeGNSS == utils::tGNSSCode::GPS || codeGNSS == utils::tGNSSCode::WAAS ? "PRN" : "ID";

	std::string Row1 = "<tr><td" + StyleHeader + ">" + StrID + "</td>";
	std::string Row2 = "<tr><td" + StyleHeader + ">Elevation</td>";
	std::string Row3 = "<tr><td" + StyleHeader + ">Azimuth</td>";
	std::string Row4 = "<tr><td" + StyleHeader + ">SNR</td>";

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
	std::string StyleSatUsed = share::GetHTMLBgColour(share::tHTMLFieldStatus::Norm);

	std::stringstream Table;
	Table << "<table>";

	Table << "<tr><td colspan=\"5\"><b>" << ToString(codeGNSS) << "</b></td></tr>";
	
	std::string StrID = codeGNSS == utils::tGNSSCode::GPS || codeGNSS == utils::tGNSSCode::WAAS ? "PRN" : "ID";
	std::string StrSatID = codeGNSS == utils::tGNSSCode::GPS || codeGNSS == utils::tGNSSCode::WAAS ? "NRD" : "CSM";

	Table << "<tr align = \"center\" " << share::GetHTMLBgColour(share::tHTMLFieldStatus::TableHeader) << "><td width=\"30\">" << StrID << "</td>";
	Table << "<td>Elv</td><td>Azm</td><td>SNR</td>";
	Table << "<td width=\"45\">Type</td>";
	Table << "<td width=\"55\">" << StrSatID << "</td></tr>";

	for (auto& i : Satellites)
	{
		if (i.GNSS != codeGNSS)
			continue;

		uint16_t CsmNrd = codeGNSS == utils::tGNSSCode::GLONASS ? i.COSMOS : i.NORAD;

		Table << "<tr align = \"center\"";
		if (i.SNR > 0)
			Table << StyleSatUsed;
		Table << "><td>"
			<< std::to_string(i.ID) << "</td><td>"
			<< std::to_string(i.Elevation) << "</td><td>"
			<< std::to_string(i.Azimuth) << "</td><td>"
			<< std::to_string(i.SNR) << "</td><td>"
			<< i.Type << "</td><td>"
			<< (CsmNrd ? std::to_string(CsmNrd) : "") << "</td></tr>";
	}

	Table << "</table>";

	return Table.str();
}

std::string tDataSetGNSS::ToString(utils::tGNSSCode codeGNSS) const
{
	switch (codeGNSS)
	{
	case utils::tGNSSCode::GLONASS: return "GLONASS";
	case utils::tGNSSCode::GPS: return "GPS";
	case utils::tGNSSCode::WAAS: return "WAAS";
	}

	return "UNKNOWN";
}

}
