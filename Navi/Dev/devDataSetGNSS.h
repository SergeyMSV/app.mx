#pragma once

#include <utilsBase.h>

#include <deque>
#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

struct tDataSetGNSS
{
	struct tSatellite
	{
		utils::tGNSSCode GNSS = utils::tGNSSCode::Unknown;
		uint8_t ID = 0;
		uint8_t Elevation = 0;
		uint16_t Azimuth = 0;
		uint8_t SNR = 0;
		std::string Type;
		uint16_t COSMOS = 0;
		uint16_t NORAD = 0;

		tSatellite() = default;
		explicit tSatellite(boost::property_tree::ptree a_PTree);

		bool operator == (const tSatellite&) const = default;
		bool operator != (const tSatellite&) const = default;

	private:
		utils::tGNSSCode GetGNSSCode(uint8_t satIDRaw) const;
		uint8_t GetGNSSSatelliteID(uint8_t satIDRaw) const;
	};

	std::string UTC = "00.00.00 00.00.00.000";
	bool Valid = false;
	double Latitude = 0;
	double Longitude = 0;
	double Altitude = 0;
	double Speed = 0;
	double Course = 0;
	std::deque<tSatellite> Satellites;

public:
	tDataSetGNSS() = default;
	explicit tDataSetGNSS(const std::string& fileName);

	bool operator == (const tDataSetGNSS&) const = default;
	bool operator != (const tDataSetGNSS&) const = default;

	void SetStateGLO(const std::string& fileName);
	void SetStateGPS(const std::string& fileName);

	std::string GetHTMLTable(const std::string& styleCol1) const;

private:
	std::string GetHTMLTableSatellitesHor(utils::tGNSSCode codeGNSS) const;
	std::string GetHTMLTableSatellitesVert(utils::tGNSSCode codeGNSS) const;

	std::string ToString(utils::tGNSSCode codeGNSS) const;

	std::string GetSatellitesQty(utils::tGNSSCode codeGNSS) const;
};

}
