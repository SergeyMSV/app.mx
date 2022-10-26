#pragma once

#include <utilsBase.h>

#include <ctime>

#include <boost/property_tree/ptree.hpp>

namespace share_gnss
{

struct tSatDesc
{
	std::uint16_t NORAD = 0;
	utils::tGNSSCode GNSS = utils::tGNSSCode::Unknown;
	std::uint16_t PRN = 0;
	std::uint8_t Slot = 0;
	std::string Plane;
	std::uint16_t SVN = 0;
	std::int8_t Channel = 0;
	std::string TypeKA;
	bool HSA = false;
	bool HSE = false;
	std::time_t HSE_DateTime = 0;
	std::uint8_t Status = 0;
	//All about satellite
	std::uint16_t COSMOS = 0;
	std::time_t Launch_Date = 0;
	std::time_t Intro_Date = 0;
	std::time_t Outage_Date = 0;
	float AESV = 0;

	tSatDesc() = default;
	explicit tSatDesc(boost::property_tree::ptree pTree);

	bool operator==(const tSatDesc& value) const = default;
	bool operator!=(const tSatDesc& value) const = default;

	std::string ToJSON() const;
};

}