#include "shareGNSS.h"

#include <iomanip>
#include <sstream>

#include <boost/property_tree/json_parser.hpp>

namespace share_gnss
{

static std::string ToString(const std::tm& time, bool showTime)
{
	std::stringstream Stream;
	if (showTime)
	{
		Stream << std::put_time(&time, "%F %T");
	}
	else
	{
		Stream << std::put_time(&time, "%F");
	}
	return Stream.str();
}

static std::string ToString(const std::time_t& timestamp, bool showTime)
{
	return ToString(*std::localtime(&timestamp), showTime);
}

std::time_t ToDateTime(const std::string& format, const std::string& value)
{
	return 0;//[TBD] linux on board supports only following locales
	//root@mx6bull:~/ temp / ram# locale - a
	//	C
	//	C.UTF - 8
	//	POSIX

	//std::tm time{};
	//std::istringstream SStream(value);
	//SStream.imbue(std::locale("us_US.utf-8"));
	//SStream >> std::get_time(&time, format.c_str());
	//if (SStream.fail())
	//	return {};

	//std::time_t posixTime = std::mktime(&time);//1970-01-01 is returned as -1
	//if (posixTime < 0)
	//	posixTime = 0;

	//return posixTime;
}

std::time_t ToDateTime(const std::string& value)
{
	return ToDateTime("%Y-%m-%d %H:%M:%S", value);
}

std::time_t ToDate(const std::string& value)
{
	return ToDateTime("%Y-%m-%d", value);
}

tSatDesc::tSatDesc(boost::property_tree::ptree pTree)
{
	NORAD = pTree.get<std::uint16_t>("norad");
	GNSS = static_cast<utils::tGNSSCode>(pTree.get<std::uint16_t>("gnss"));
	PRN = pTree.get<std::uint16_t>("prn");
	Slot = pTree.get<std::int8_t>("slot");
	Plane = pTree.get<std::string>("plane");
	SVN = pTree.get<std::uint16_t>("svn");
	Channel = pTree.get<std::int8_t>("channel");
	TypeKA = pTree.get<std::string>("type_ka");
	HSA = pTree.get<bool>("hsa");
	HSE = pTree.get<bool>("hse");
	HSE_DateTime = ToDateTime(pTree.get<std::string>("hse_datetime"));
	Status = pTree.get<std::uint8_t>("status");
	COSMOS = pTree.get<std::uint16_t>("cosmos");
	Launch_Date = ToDate(pTree.get<std::string>("launch_date"));
	Intro_Date = ToDate(pTree.get<std::string>("intro_date"));
	Outage_Date = ToDate(pTree.get<std::string>("outage_date"));
	AESV = pTree.get<float>("aesv");
}

std::string tSatDesc::ToJSON() const
{
	std::string Str = "{";
	Str += "\n\"norad\": " + std::to_string(NORAD);
	Str += ",\n\"gnss\": " + std::to_string(static_cast<int>(GNSS));
	Str += ",\n\"prn\": " + std::to_string(PRN);
	Str += ",\n\"slot\": " + std::to_string(Slot);
	Str += ",\n\"plane\": \"" + Plane + "\"";
	Str += ",\n\"svn\": " + std::to_string(SVN);
	Str += ",\n\"channel\": " + std::to_string(Channel);
	Str += ",\n\"type_ka\": \"" + TypeKA + "\"";
	Str += ",\n\"hsa\": " + std::to_string(HSA);
	Str += ",\n\"hse\": " + std::to_string(HSE);
	Str += ",\n\"hse_datetime\": \"" + ToString(HSE_DateTime, true) + "\"";
	Str += ",\n\"status\": " + std::to_string(Status);
	Str += ",\n\"cosmos\": " + std::to_string(COSMOS);
	Str += ",\n\"launch_date\": \"" + ToString(Launch_Date, false) + "\"";
	Str += ",\n\"intro_date\": \"" + ToString(Intro_Date, false) + "\"";
	Str += ",\n\"outage_date\": \"" + ToString(Outage_Date, false) + "\"";
	std::stringstream SStr;
	SStr.setf(std::ios::fixed);
	SStr.precision(1);
	SStr << AESV;
	Str += ",\n\"aesv\": " + SStr.str();
	Str += "\n}";
	return Str;
}

}