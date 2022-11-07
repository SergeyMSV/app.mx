#include <devConfig.h>

#include <shareGNSS.h>

#include <ctime>

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <boost/property_tree/json_parser.hpp>

std::time_t ToTime(const std::string& format, const std::string& value)
{
	return 0;//[TBD] linux on board supports only following locales
	//root@mx6bull:~/ temp / ram# locale - a
	//	C
	//	C.UTF - 8
	//	POSIX
 
	//std::tm time{};
	//std::istringstream SStream(value);
	//SStream.imbue(std::locale("en_US.utf-8"));
	//SStream >> std::get_time(&time, format.c_str());
	//if (SStream.fail())
	//	return {};

	//return std::mktime(&time);
}

std::string ParseGLO(const boost::property_tree::ptree& pTree)
{
	std::string StrJSON = "\"glonass\": [\n";

	bool FirstItem = true;
	for (auto& i : pTree.get_child(""))
	{
		if (FirstItem)
		{
			FirstItem = false;
		}
		else
		{
			StrJSON += ",\n";
		}

		share_gnss::tSatDesc SatDesc{};

		SatDesc.GNSS = utils::tGNSSCode::GLONASS;
		SatDesc.Slot = i.second.get<uint8_t>("point", 0);
		SatDesc.Plane = i.second.get<std::string>("plane");
		SatDesc.SVN = i.second.get<uint16_t>("SVN", 0);
		SatDesc.Channel = i.second.get<int8_t>("slot", 0x80);//in case of empty value (" ") it sets 0x80 = -128
		SatDesc.NORAD = i.second.get<uint16_t>("NORAD", 0);
		SatDesc.COSMOS = i.second.get<uint16_t>("COSMOS", 0);
		SatDesc.TypeKA = i.second.get<std::string>("typeKA");
		SatDesc.HSA = i.second.get<std::string>("HSA") == "+";

		std::string StrHSE = i.second.get<std::string>("HSE");
		if (StrHSE.size() == 16)
		{
			SatDesc.HSE = StrHSE[0] == '+';
			SatDesc.HSE_DateTime = ToTime("%H:%M %d.%m.%y", StrHSE.c_str() + 2);
		}

		std::string StrDL = i.second.get<std::string>("DL");
		SatDesc.Launch_Date = ToTime("%d.%m.%y", StrDL.c_str());

		std::string StrDC = i.second.get<std::string>("DC");
		SatDesc.Intro_Date = ToTime("%d.%m.%y", StrDC.c_str());

		std::string StrDDC = i.second.get<std::string>("DDC");
		SatDesc.Outage_Date = ToTime("%d.%m.%y", StrDDC.c_str());

		SatDesc.AESV = i.second.get<float>("AESV", 0);
		SatDesc.Status = i.second.get<uint8_t>("status", 0);

		StrJSON += SatDesc.ToJSON();
	}

	StrJSON += "\n]";

	return StrJSON;
}

std::string ParseGPS(const boost::property_tree::ptree& pTree)
{
	std::string StrJSON = "\"gps\": [\n";

	bool FirstItem = true;
	for (auto& i : pTree.get_child(""))
	{
		if (FirstItem)
		{
			FirstItem = false;
		}
		else
		{
			StrJSON += ",\n";
		}

		//"point":"1",
		//"datetime":"28.09.21",
		//"PRN":"24",
		//"plane":"A",
		//"DL":"04.10.12",
		//"DC":"14.11.12",
		//"DDC":" ",
		//"AESV":"106.5",
		//"typeID":"1",
		//"comment":"\u0418\u0441\u043f\u043e\u043b\u044c\u0437\u0443\u0435\u0442\u0441\u044f \u043f\u043e \u0426\u041d",
		//"typeKA":"II-F",
		//"NORAD":"38833",
		//"color":"#ffffff"

		share_gnss::tSatDesc SatDesc{};

		SatDesc.GNSS = utils::tGNSSCode::GPS;

		SatDesc.Channel = i.second.get<uint8_t>("point", 0);//channel may be wrong
		SatDesc.PRN = i.second.get<uint16_t>("PRN", 0);//in case of empty value (" ") it sets 0
		SatDesc.Plane = i.second.get<std::string>("plane");
		SatDesc.NORAD = i.second.get<uint16_t>("NORAD", 0);
		SatDesc.TypeKA = i.second.get<std::string>("typeKA");

		std::string StrDL = i.second.get<std::string>("DL");
		SatDesc.Launch_Date = ToTime("%d.%m.%y", StrDL.c_str());

		std::string StrDC = i.second.get<std::string>("DC");
		SatDesc.Intro_Date = ToTime("%d.%m.%y", StrDC.c_str());

		std::string StrDDC = i.second.get<std::string>("DDC");
		SatDesc.Outage_Date = ToTime("%d.%m.%y", StrDDC.c_str());

		SatDesc.AESV = i.second.get<float>("AESV", 0);//in case of empty value (" ") it sets 0

		StrJSON += SatDesc.ToJSON();
	}

	StrJSON += "\n]";

	return StrJSON;
}
