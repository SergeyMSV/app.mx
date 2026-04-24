///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketNMEAPayload
// 2020-01-31, 2026-01-17 refact.
// C++17
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsPacketNMEA.h"
#include "utilsPacketNMEAType.h"

namespace utils
{
namespace packet
{
namespace nmea
{
namespace base
{
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
	typename TTime,
	typename TLatitude,
	typename TLongitude,
	typename TQuality,
	typename TSatQty,
	typename THDOP,
	typename TAltitude,
	typename TGeoidSeparation,
	typename TDiffAge,
	typename TDiffStation
>
struct tContentGGA : public type::tTypeVerified	// Global Positioning System Fix Data
{
	type::tGNSS GNSS;
	TTime Time;								// UTC Time
	TLatitude Latitude;
	TLongitude Longitude;
	TQuality Quality = TQuality(0);			// GPS Quality Indicator (Position Fix Indicator, 1 digit)
	TSatQty SatUsed = TSatQty(0);			// Satellites Used
	THDOP HDOP;								// Horizontal Dilution of Precision 
	TAltitude Altitude;						// Altitude re: mean-sea-level (geoid), meters
	TGeoidSeparation GeoidSeparation;		// Geoidal Separation: the difference between the WGS-84 earth ellipsoid surface and mean-sea-level (geoid) surface, "-" = mean-sea-level surface below WGS - 84 ellipsoid surface.
	TDiffAge DiffAge;						// Age of Differential Corrections
	TDiffStation DiffStation;				// Diff. Reference Station ID

	tContentGGA() = default;
	explicit tContentGGA(const std::vector<std::string>& val)
	{
		if (val.size() != 15 || val[0].size() < 3 || std::strcmp(&val[0][2], GetID()))
		{
			SetVerified(false);
			return;
		}
		GNSS = type::tGNSS(val[0]);
		Time = TTime(val[1]);
		Latitude = TLatitude(val[2], val[3]);
		Longitude = TLongitude(val[4], val[5]);
		Quality = TQuality(val[6]);
		SatUsed = TSatQty(val[7]);
		HDOP = THDOP(val[8]);
		Altitude = TAltitude(val[9], val[10]);
		GeoidSeparation = TGeoidSeparation(val[11], val[12]);
		DiffAge = TDiffAge(val[13]);
		DiffStation = TDiffStation(val[14]);
	}

	static const char* GetID() { return "GGA"; }

	bool IsVerified() const { return type::tTypeVerified::IsVerified() && type::IsVerified(GNSS, Time, Latitude, Longitude, Quality, SatUsed, HDOP, Altitude, GeoidSeparation, DiffAge, DiffStation); }

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data;
		Data.push_back(GNSS.ToString() + GetID());
		Data.push_back(Time.ToString());
		Data.push_back(Latitude.ToStringValue());
		Data.push_back(Latitude.ToStringHemisphere());
		Data.push_back(Longitude.ToStringValue());
		Data.push_back(Longitude.ToStringHemisphere());
		Data.push_back(Quality.ToString());
		Data.push_back(SatUsed.ToString());
		Data.push_back(HDOP.ToString());
		Data.push_back(Altitude.ToStringValue());
		Data.push_back(Altitude.ToStringUnit());
		Data.push_back(GeoidSeparation.ToStringValue());
		Data.push_back(GeoidSeparation.ToStringUnit());
		Data.push_back(DiffAge.ToString());
		Data.push_back(DiffStation.ToString());
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
	typename TLatitude,
	typename TLongitude,
	typename TTime
>
struct tContentGLL7 : public type::tTypeVerified	// Geographic Position - Latitude/Longitude
{
	type::tGNSS GNSS;
	TLatitude Latitude;
	TLongitude Longitude;
	TTime Time;								// UTC Time
	type::tStatusNoNull Status;				// A = Data valid, V = Navigation receiver warning

	tContentGLL7() = default;
	explicit tContentGLL7(const std::vector<std::string>& val)
	{
		if (val.size() < 7 || val[0].size() < 3 || std::strcmp(&val[0][2], GetID()))
		{
			SetVerified(false);
			return;
		}
		GNSS = type::tGNSS(val[0]);
		Latitude = TLatitude(val[1], val[2]);
		Longitude = TLongitude(val[3], val[4]);
		Time = TTime(val[5]);
		Status = type::tStatusNoNull(val[6]);
	}

	static const char* GetID() { return "GLL"; }

	bool IsVerified() const { return type::tTypeVerified::IsVerified() && type::IsVerified(GNSS, Latitude, Longitude, Time, Status); }

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data;
		Data.push_back(GNSS.ToString() + GetID());
		Data.push_back(Latitude.ToStringValue());
		Data.push_back(Latitude.ToStringHemisphere());
		Data.push_back(Longitude.ToStringValue());
		Data.push_back(Longitude.ToStringHemisphere());
		Data.push_back(Time.ToString());
		Data.push_back(Status.ToString());
		return Data;
	}
};

template
<
	typename TLatitude,
	typename TLongitude,
	typename TTime,
	typename TMode
>
struct tContentGLL8 : public tContentGLL7<TLatitude, TLongitude, TTime> // NMEA version 2.3 and later
{
	using tBase = tContentGLL7<TLatitude, TLongitude, TTime>;

	TMode Mode;								// Positioning system Mode Indicator

	tContentGLL8() = default;
	explicit tContentGLL8(const std::vector<std::string>& val)
		:tBase(val)
	{
		if (val.size() != 8)
		{
			tBase::SetVerified(false);
			return;
		}
		Mode = TMode(val[7]);
	}

	bool IsVerified() const { return tBase::IsVerified() && type::IsVerified(Mode); }

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data = tBase::GetPayload();
		Data.push_back(Mode.ToString());
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
	typename TSatID,
	typename THDOP
>
struct tContentGSA : public type::tTypeVerified	// GNSS DOP and Active Satellites
{
	using tFixStatus = type::tUIntFixedNoNull<1>;

	static constexpr std::size_t ChannelQty = 12;

	type::tGNSS GNSS;
	char OperMode = 0;						// M = Manual, forced to operate in 2D or 3D mode; A = Automatic, allowed to automatically switch 2D/3D
	tFixStatus FixStatus = tFixStatus(0);	// 1 = Fix not available, 2 = 2D, 3 = 3D
	TSatID SatsOnChannels[ChannelQty];		// ID numbers of satellites used in solution
	THDOP PDOP;
	THDOP HDOP;
	THDOP VDOP;

	tContentGSA() = default;
	explicit tContentGSA(const std::vector<std::string>& val)
	{
		if (val.size() != 18 || val[0].size() < 3 || std::strcmp(&val[0][2], GetID()) || val[1].size() != 1 || val[2].size() != 1)
		{
			SetVerified(false);
			return;
		}
		GNSS = type::tGNSS(val[0]);
		OperMode = val[1][0];
		FixStatus = tFixStatus(val[2]);
		for (std::size_t i = 0; i < ChannelQty; ++i)
		{
			SatsOnChannels[i] = TSatID(val[3 + i]);
		}
		PDOP = THDOP(val[15]);
		HDOP = THDOP(val[16]);
		VDOP = THDOP(val[17]);
	}

	static const char* GetID() { return "GSA"; }

	bool IsVerified() const
	{
		bool Verified = type::tTypeVerified::IsVerified() && type::IsVerified(GNSS, FixStatus, PDOP, HDOP, VDOP) &&
			(OperMode == 'M' || OperMode == 'A') && FixStatus.GetValue() <= 3;
		for (const auto& i : SatsOnChannels)
		{
			if (!i.IsVerified())
				Verified = false;
		}
		return Verified;
	}

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data;
		Data.push_back(GNSS.ToString() + GetID());
		Data.push_back(OperMode ? std::string(1, OperMode) : "");
		Data.push_back(FixStatus.ToString());
		for (const auto& i : SatsOnChannels)
		{
			Data.push_back(i.ToString());
		}
		Data.push_back(PDOP.ToString());
		Data.push_back(HDOP.ToString());
		Data.push_back(VDOP.ToString());
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
	typename TMsgQty,
	typename TMsgNum,
	typename TSatQty,
	typename TSatID,
	typename TElevation,
	typename TAzimuth,
	typename TSNR
>
struct tContentGSV : public type::tTypeVerified	// GNSS Satellites in View
{
	type::tGNSS GNSS;
	TMsgQty MsgQty;							// Total number of messages, 1 to 9
	TMsgNum MsgNum;							// Message number, 1 to 9
	TSatQty SatInView;						// Satellites in View 

	struct tSatellite
	{
		TSatID ID;							// Satellite ID (GPS: 1-32, SBAS 33-64 (33=PRN120), GLONASS: 65-96) 
		TElevation Elevation;				// Degree (Maximum 90)
		TAzimuth Azimuth;					// Degree (Range 0 to 359)
		TSNR SNR;							// SNR (C/No) 00-99 dB-Hz, null when not tracking
	};
	std::vector<tSatellite> Sats;

	tContentGSV() = default;
	explicit tContentGSV(const std::vector<std::string>& val)
	{
		if (val.size() < 4 || val[0].size() < 3 || std::strcmp(&val[0][2], GetID()))
		{
			SetVerified(false);
			return;
		}
		GNSS = type::tGNSS(val[0]);
		MsgQty = TMsgQty(val[1]);
		MsgNum = TMsgNum(val[2]);
		SatInView = TSatQty(val[3]);
		const int SatQtyMsg = static_cast<int>((val.size() - 3) / 4);
		for (int i = 1; i <= SatQtyMsg; ++i)
		{
			int Index = 4 * i;
			tSatellite Sat{};
			Sat.ID = TSatID(val[Index]);
			Sat.Elevation = TElevation(val[Index + 1]);
			Sat.Azimuth = TAzimuth(val[Index + 2]);
			Sat.SNR = TSNR(val[Index + 3]);
			Sats.push_back(Sat);
		}
	}

	static const char* GetID() { return "GSV"; }

	bool IsVerified() const
	{
		if (!type::tTypeVerified::IsVerified())
			return false;
		bool Verified = type::IsVerified(GNSS, MsgQty, MsgNum, SatInView);
		for (auto& i : Sats)
		{
			Verified = type::IsVerified(i.ID, i.Elevation, i.Azimuth, i.SNR);
		}
		return Verified;
	}

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data;
		Data.push_back(GNSS.ToString() + GetID());
		Data.push_back(MsgQty.ToString());
		Data.push_back(MsgNum.ToString());
		Data.push_back(SatInView.ToString());
		for (auto& i : Sats)
		{
			Data.push_back(i.ID.ToString());
			Data.push_back(i.Elevation.ToString());
			Data.push_back(i.Azimuth.ToString());
			Data.push_back(i.SNR.ToString());
		}
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
	typename TTime,
	typename TLatitude,
	typename TLongitude,
	typename TSpeed,
	typename TCourse,
	typename TDate
>
struct tContentRMC12 : public type::tTypeVerified	// Recommended Minimum Specific GNSS Data
{
	type::tGNSS GNSS;
	TTime Time;								// UTC Time
	type::tStatusNoNull Status;				// A = Data valid, V = Navigation receiver warning
	TLatitude Latitude;
	TLongitude Longitude;
	TSpeed Speed;							// Speed Over Ground, knots
	TCourse Course;							// Course Over Ground, degrees
	TDate Date;

	tContentRMC12() = default;
	explicit tContentRMC12(const std::vector<std::string>& val)
	{
		if (val.size() < 12 ||
			val[0].size() < 3 || std::strcmp(&val[0][2], GetID())
			|| !val[10].empty() || !val[11].empty()) // 10 and 11 stand for Magnetic variation (not supported by receiver))
		{
			SetVerified(false);
			return;
		}
		GNSS = type::tGNSS(val[0]);
		Time = TTime(val[1]);
		Status = type::tStatusNoNull(val[2]);
		Latitude = TLatitude(val[3], val[4]);
		Longitude = TLongitude(val[5], val[6]);
		Speed = TSpeed(val[7]);
		Course = TCourse(val[8]);
		Date = TDate(val[9]);
	}

	static const char* GetID() { return "RMC"; }

	bool IsVerified() const { return type::tTypeVerified::IsVerified() && type::IsVerified(GNSS, Time, Status, Latitude, Longitude, Speed, Course, Date); }

	std::pair<std::time_t, std::uint32_t> GetDateTime() const
	{
		auto [DtSec, Milliseconds] = utils::packet::nmea::type::hidden::SplitDouble(Time.GetValue(), 3); // 3 -> milliseconds
		std::time_t DateTime = Date.GetValue() + DtSec;
		return { DateTime, Milliseconds };
	}

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data;
		Data.push_back(GNSS.ToString() + GetID());
		Data.push_back(Time.ToString());
		Data.push_back(Status.ToString());
		Data.push_back(Latitude.ToStringValue());
		Data.push_back(Latitude.ToStringHemisphere());
		Data.push_back(Longitude.ToStringValue());
		Data.push_back(Longitude.ToStringHemisphere());
		Data.push_back(Speed.ToString());
		Data.push_back(Course.ToString());
		Data.push_back(Date.ToString());
		Data.push_back("");
		Data.push_back("");
		return Data;
	}
};

template
<
	typename TTime,
	typename TLatitude,
	typename TLongitude,
	typename TSpeed,
	typename TCourse,
	typename TDate,
	typename TMode
>
struct tContentRMC13 : public tContentRMC12<TTime, TLatitude, TLongitude, TSpeed, TCourse, TDate> // NMEA version 2.3 and later
{
	using tBase = tContentRMC12<TTime, TLatitude, TLongitude, TSpeed, TCourse, TDate>;

	TMode Mode;								// Positioning system Mode Indicator

	tContentRMC13() = default;
	explicit tContentRMC13(const std::vector<std::string>& val)
		:tBase(val)
	{
		if (val.size() != 13)
		{
			tBase::SetVerified(false);
			return;
		}
		Mode = TMode(val[12]);
	}

	bool IsVerified() const { return tBase::IsVerified() && type::IsVerified(Mode); }

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data = tBase::GetPayload();
		Data.push_back(Mode.ToString());
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
	typename TCourse,
	typename TSpeed
>
struct tContentVTG9 : public type::tTypeVerified	// Course Over Ground and Ground Speed
{
	type::tGNSS GNSS;
	TCourse Course;							// Course Over Ground, degrees
	TSpeed Speed;							// Speed Over Ground, knots
	TSpeed SpeedKmH;						// Speed Over Ground, km/h

	tContentVTG9() = default;
	explicit tContentVTG9(const std::vector<std::string>& val)
	{
		if (val.size() < 9 || val[0].size() < 3 || std::strcmp(&val[0][2], GetID()) ||
			val[2] != "T" || val[3] != "" || val[4] != "M" || val[6] != "N" || val[8] != "K")
		{
			SetVerified(false);
			return;
		}
		GNSS = type::tGNSS(val[0]);
		Course = TCourse(val[1]);
		Speed = TSpeed(val[5]);
		SpeedKmH = TSpeed(val[7]);
	}

	static const char* GetID() { return "VTG"; }

	bool IsVerified() const { return type::tTypeVerified::IsVerified() && type::IsVerified(GNSS, Course, Speed, SpeedKmH); }

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data;
		Data.push_back(GNSS.ToString() + GetID());
		Data.push_back(Course.ToString());
		Data.push_back("T");
		Data.push_back("");
		Data.push_back("M");
		Data.push_back(Speed.ToString());
		Data.push_back("N");
		Data.push_back(SpeedKmH.ToString());
		Data.push_back("K");
		return Data;
	}
};

template
<
	typename TCourse,
	typename TSpeed,
	typename TMode
>
struct tContentVTG10 : public tContentVTG9<TCourse, TSpeed>
{
	using tBase = tContentVTG9<TCourse, TSpeed>;

	TMode Mode;								// Positioning system Mode Indicator

	tContentVTG10() = default;
	explicit tContentVTG10(const std::vector<std::string>& val)
		:tBase(val)
	{
		if (val.size() != 10)
		{
			tBase::SetVerified(false);
			return;
		}
		Mode = TMode(val[9]);
	}

	bool IsVerified() const { return tBase::IsVerified() && type::IsVerified(Mode); }

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data = tBase::GetPayload();
		Data.push_back(Mode.ToString());
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
template
<
	typename TTime,
	typename TDay,
	typename TMonth,
	typename TYear,
	typename TLocalZoneHours,
	typename TLocalZoneMinutes
>
struct tContentZDA : public type::tTypeVerified	// Time & Date
{
	type::tGNSS GNSS;
	TTime Time;								// UTC Time
	TDay Day;								// Day, 01 to 31
	TMonth Month;							// Month, 01 to 12
	TYear Year;								// Year, (for MTK 3333 from 1980 to 2079)
	TLocalZoneHours LocalZoneHours;			// Offset from UTC, 00 to ±13 hrs
	TLocalZoneMinutes LocalZoneMinutes;		// Offset from UTC, 00 to +59

	tContentZDA() = default;
	explicit tContentZDA(const std::vector<std::string>& val)
	{
		if (val.size() != 7 || val[0].size() < 3 || std::strcmp(&val[0][2], GetID()))
		{
			SetVerified(false);
			return;
		}
		GNSS = type::tGNSS(val[0]);
		Time = TTime(val[1]);
		Day = TDay(val[2]);
		Month = TMonth(val[3]);
		Year = TYear(val[4]);
		LocalZoneHours = TLocalZoneHours(val[5]);
		LocalZoneMinutes = TLocalZoneMinutes(val[6]);
	}

	static const char* GetID() { return "ZDA"; }

	bool IsVerified() const { return type::tTypeVerified::IsVerified() && type::IsVerified(GNSS, Time, Day, Month, Year, LocalZoneHours, LocalZoneMinutes); }

	std::pair<std::time_t, std::uint32_t> GetDateTime() const
	{
		auto [DtSec, Milliseconds] = utils::packet::nmea::type::hidden::SplitDouble(Time.GetValue(), 3); // 3 -> milliseconds
		tm DT{};
		DT.tm_year = Year.GetValue() - 1900;
		DT.tm_mon = Month.GetValue() - 1;
		DT.tm_mday = Day.GetValue();
		std::time_t DateTime = mktime(&DT);
		return { DateTime + DtSec, Milliseconds };
	}

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data;
		Data.push_back(GNSS.ToString() + GetID());
		Data.push_back(Time.ToString());
		Data.push_back(Day.ToString());
		Data.push_back(Month.ToString());
		Data.push_back(Year.ToString());
		Data.push_back(LocalZoneHours.ToString());
		Data.push_back(LocalZoneMinutes.ToString());
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T, typename TArg>
std::optional<T> Parse(const TArg& value)
{
	T Pack = T(value);
	if (!Pack.IsVerified())
		return {};
	return Pack;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace generic
{
	using tMsgQty = type::tUIntFixedNoNull<1>;						// 0		Total number of messages, 1 to 9
	using tMsgNum = type::tUIntFixedNoNull<1>;						// 0		Message number, 1 to 9
	using tSatInViewQty = type::tUIntFixedNoNull<2>;				// 00 - 99
	using tSatID = type::tUIntFixedNoNull<2>;						// 00		Satellite ID (GPS: 1-32, SBAS 33-64 (33=PRN120), GLONASS: 65-96) 
	using tElevation = type::tUIntFixedNoNull<2>;					// 00		Degree (Maximum 90)
	using tAzimuth = type::tUIntFixedNoNull<3>;						// 000		Degree (Range 0 to 359)
	using tSNR = type::tUIntFixed<2>;								// 00		SNR (C/No) 00-99 dB-Hz, null when not tracking

	using tContentGSV = base::tContentGSV<tMsgQty, tMsgNum, tSatInViewQty, tSatID, tElevation, tAzimuth, tSNR>;
}

namespace mtk_eb500 // MT3329	AXN_1.30
{
	// Not valid
	// $GPGGA,000000.000,0000.0000,N,00000.0000,E,0,0,,1.0,M,10.0,M,,*??
	// $GPRMC,000000.000,V,0000.0000,N,00000.0000,E,0.00,0.00,000000,,,N*??
	//
	// Valid (DIFF)
	// $GPGGA,000000.000,0000.0000,N,00000.0000,E,0,0,1.23,123.4,M,12.3,M,0000,0000*??
	// $GPRMC,000000.000,A,0000.0000,N,00000.0000,E,123.00,123.00,000000,,,D*??
	//
	// $GPGSV,1,1,00*79
	// $GPGSV,4,1,13,01,01,001,,02,02,002,22,03,33,003,33,04,04,004,*??

	using tTime = type::tTimeNoNull<3>;										// 000000.000
	using tDate = type::tDateNoNull;
	using tLatitude = type::tLatitudeNoNull<4>;								// 0000.0000,?
	using tLongitude = type::tLongitudeNoNull<4>;							// 00000.0000,?
	using tQuality = type::tQualityNoNull;									// 0
	using tSatQty = type::tUIntNoNull<2>;									// 0 - 99
	using tHDOP = type::tFloatPrecisionFixed<2, 2>;							// ?.00
	using tAltitude = type::tFloatPrecisionFixedUnitNoNull<5, 1>;			// ?.0,M			[?] tFloatPrecisionFixed maybe NoNull too
	using tGeoidSeparation = type::tFloatPrecisionFixedUnitNoNull<4, 1>;	// ?.0,M			[?] tFloatPrecisionFixed maybe NoNull too
	using tDiffAge = type::tFloatPrecisionFixed<3, 1>;						// 0.0 - 999.9				[?]
	using tDiffStation = type::tUIntFixed<4>;								// 0000 - 9999
	using tSpeed = type::tFloatPrecisionFixedNoNull<4, 2>;					// 0.00 - 9999.99
	using tCourse = type::tFloatPrecisionFixedNoNull<3, 2>;					// 0.00 - 999.99
	using tMode = type::tModeNoNull;										// A

	using tContentGGA = base::tContentGGA<tTime, tLatitude, tLongitude, tQuality, tSatQty, tHDOP, tAltitude, tGeoidSeparation, tDiffAge, tDiffStation>;
	using tContentGSV = generic::tContentGSV;
	using tContentRMC = base::tContentRMC13<tTime, tLatitude, tLongitude, tSpeed, tCourse, tDate, tMode>;
}

namespace mtk_eb800a // MT3339	AXN_3.8
{
	//$GNGLL,,,,,000106.799,V,N*64
	//$GNZDA,000106.799,06,01,1980,,*4F

	using tTime = type::tTimeNoNull<3>;										// 000000.000
	using tDate = type::tDateNoNull;
	using tLatitude = type::tLatitude<6>;									// 0000.000000,?
	using tLongitude = type::tLongitude<6>;									// 00000.000000,?
	using tQuality = type::tQualityNoNull;									// 0
	using tSatQty = type::tUIntNoNull<2>;									// 0 - 99
	using tSatID = type::tUIntFixed<2>;										// 00		Satellite ID (GPS: 1-32, SBAS 33-64 (33=PRN120), GLONASS: 65-96) 
	using tHDOP = type::tFloatPrecisionFixed<2, 2>;							// ?.00
	using tAltitude = type::tFloatPrecisionFixedUnitNoNull<5, 3>;			// ?.000,M
	using tGeoidSeparation = type::tFloatPrecisionFixedUnitNoNull<4, 3>;	// ?.000,M
	using tDiffAge = type::tFloatPrecisionFixed<3, 1>;						// 0.0 - 999.9			[?]
	using tDiffStation = type::tUIntFixed<4>;								// 0000 - 9999
	using tSpeed = type::tFloatPrecisionFixedNoNull<4, 2>;					// 0.00 - 9999.99
	using tCourse = type::tFloatPrecisionFixedNoNull<3, 2>;					// 0.00 - 999.99
	using tMode = type::tModeNoNull;										// A
	using tDay = type::tUIntFixedNoNull<2>;									// 00
	using tMonth = type::tUIntFixedNoNull<2>;								// 00
	using tYear = type::tUIntFixedNoNull<4>;								// 1980
	using tLocalZoneHours = type::tIntFixed<2>;								// 00 to ±13 hrs
	using tLocalZoneMinutes = type::tUIntFixed<2>;							// 00 to +59

	using tContentGGA = base::tContentGGA<tTime, tLatitude, tLongitude, tQuality, tSatQty, tHDOP, tAltitude, tGeoidSeparation, tDiffAge, tDiffStation>;
	using tContentGLL = base::tContentGLL8<tLatitude, tLongitude, tTime, tMode>;
	using tContentGSA = base::tContentGSA<tSatID, tHDOP>;
	using tContentGSV = generic::tContentGSV;
	using tContentRMC = base::tContentRMC13<tTime, tLatitude, tLongitude, tSpeed, tCourse, tDate, tMode>;
	using tContentVTG = base::tContentVTG10<tCourse, tSpeed, tMode>;
	using tContentZDA = base::tContentZDA<tTime, tDay, tMonth, tYear, tLocalZoneHours, tLocalZoneMinutes>;
}
//namespace mtk_axn_3_10
//{
// 
// 
//}
namespace mtk_sc872_a // MT3333	AXN_3.84
{
	// $GNGGA,235949.799,,,,,0,0,,,M,,M,,*51
	// $GPGSV,1,1,04,01,,,44,02,,,30,03,,,45,17,,,38*72
	// $GLGSV,1,1,00*65
	// $GNRMC,235949.799,V,,,,,0.00,0.00,050180,,,N*58
	// $GNGGA,235950.277,,,,,0,0,,,M,,M,,*5C
	// $GPGSV,1,1,04,01,,,44,02,,,31,03,,,45,04,,,04*??
	// $GLGSV,1,1,00*65

	// $GNRMC,225721.000,A,1234.567890,N,01234.567890,E,0.80,337.72,120126,,,A*??
	// $GNGGA,225722.000,1234.567890,N,01234.567890,E,1,4,1.23,123.456,M,12.456,M,,*??
	// $GPGSV,2,1,06,01,01,001,01,02,02,002,02,03,03,003,,04,,,04*??
	// $GPGSV,2,2,06,05,,,05,06,,,06*??
	// $GLGSV,2,1,05,65,65,065,65,66,66,066,66,67,67,067,,68,68,068,68*??
	// $GLGSV,2,2,05,69,69,069,69*??

	//$GNRMC,000015.799,V,,,,,0.00,0.00,060180,,,N*5F
	//$GNVTG,0.00,T,,M,0.00,N,0.00,K,N*2C
	//$PTWS,JAM,SIGNAL,VAL,INDEX,9,FREQ,1575.135010*66
	//$PTWS,JAM,SIGNAL,VAL,INDEX,10,FREQ,1575.264771*59
	//$GNGGA,000016.799,,,,,0,0,,,M,,M,,*56
	//$GPGSA,A,1,,,,,,,,,,,,,,,*1E
	//$GLGSA,A,1,,,,,,,,,,,,,,,*02
	//$GPGSV,1,1,00*79
	//$GLGSV,1,1,00*65
	//$GNRMC,000016.799,V,,,,,0.00,0.00,060180,,,N*5C
	//$PTWS,JAM,SIGNAL,VAL,INDEX,8,FREQ,1575.198853*6F
	//$PTWS,JAM,SIGNAL,VAL,INDEX,9,FREQ,1575.133911*68
	//$PTWS,JAM,SIGNAL,VAL,INDEX,10,FREQ,1575.264771*59
	//$GNGGA,000017.799,,,,,0,0,,,M,,M,,*57
	//$GPGSA,A,1,,,,,,,,,,,,,,,*1E
	//$GLGSA,A,1,,,,,,,,,,,,,,,*02

	//$GNVTG,0.00,T,,M,0.00,N,0.00,K,N*2C
	//$GNVTG,87.26,T,,M,2.16,N,4.01,K,A*18

	using tTime = type::tTimeNoNull<3>;										// 000000.000
	using tDate = type::tDateNoNull;
	using tLatitude = type::tLatitude<4>;									// 0000.0000,?
	using tLongitude = type::tLongitude<4>;									// 00000.0000,?
	using tQuality = type::tQualityNoNull;									// 0
	using tSatQty = type::tUIntNoNull<2>;									// 0 - 99
	using tSatID = type::tUIntFixed<2>;										// 00		Satellite ID (GPS: 1-32, SBAS 33-64 (33=PRN120), GLONASS: 65-96) 
	using tHDOP = type::tFloatPrecisionFixed<2, 2>;							// ?.00
	using tAltitude = type::tFloatPrecisionFixedUnitNoNull<5, 1>;			// ?.0,M
	using tGeoidSeparation = type::tFloatPrecisionFixedUnitNoNull<4, 1>;	// ?.0,M
	using tDiffAge = type::tFloatPrecisionFixed<3, 1>;						// 0.0 - 999.9			[?]
	using tDiffStation = type::tUIntFixed<4>;								// 0000 - 9999
	using tSpeed = type::tFloatPrecisionFixedNoNull<4, 2>;					// 0.00 - 9999.99
	using tCourse = type::tFloatPrecisionFixedNoNull<3, 2>;					// 0.00 - 999.99
	using tMode = type::tModeNoNull;										// A
	using tDay = type::tUIntFixedNoNull<2>;									// 00
	using tMonth = type::tUIntFixedNoNull<2>;								// 00
	using tYear = type::tUIntFixedNoNull<4>;								// 1980
	using tLocalZoneHours = type::tIntFixed<2>;								// 00 to ±13 hrs
	using tLocalZoneMinutes = type::tUIntFixed<2>;							// 00 to +59

	using tContentGGA = base::tContentGGA<tTime, tLatitude, tLongitude, tQuality, tSatQty, tHDOP, tAltitude, tGeoidSeparation, tDiffAge, tDiffStation>;
	using tContentGLL = base::tContentGLL8<tLatitude, tLongitude, tTime, tMode>;
	using tContentGSA = base::tContentGSA<tSatID, tHDOP>;
	using tContentGSV = generic::tContentGSV;
	using tContentRMC = base::tContentRMC13<tTime, tLatitude, tLongitude, tSpeed, tCourse, tDate, tMode>;
	using tContentVTG = base::tContentVTG9<tCourse, tSpeed>;
	using tContentZDA = base::tContentZDA<tTime, tDay, tMonth, tYear, tLocalZoneHours, tLocalZoneMinutes>;
}

namespace sirf_gsu_7x
{
	using tTime = type::tTimeNoNull<3>;									// 000000.000
	using tDate = type::tDateNoNull;
	using tLatitude = type::tLatitudeNoNull<4>;							// 0000.0000,?
	using tLongitude = type::tLongitudeNoNull<4>;						// 00000.0000,?
	using tQuality = type::tQualityNoNull;								// 0
	using tSatQty = type::tUIntFixedNoNull<2>;							// 00
	using tSatID = type::tUIntFixed<2>;									// 00		Satellite ID (GPS: 1-32, SBAS 33-64 (33=PRN120), GLONASS: 65-96) 
	using tHDOP = type::tFloatFixedNoNull<2, 1>;						// 00.0
	using tAltitude = type::tFloatFixedNoNullUnitNoNull<5, 1>;			// 00000.0,M		-0014.2,M
	using tGeoidSeparation = type::tFloatFixedNoNullUnitNoNull<4, 1>;	// 0000.0,M			-014.2,M
	using tDiffAge = type::tFloatFixedNoNull<3, 1>;						// 000.0
	using tDiffStation = type::tUIntFixedNoNull<4>;						// 0000
	using tSpeed = type::tFloatFixedNoNull<4, 2>;						// 9999.99
	using tCourse = type::tFloatFixedNoNull<3, 2>;						// 999.99
	using tDay = type::tUIntFixedNoNull<2>;								// 00
	using tMonth = type::tUIntFixedNoNull<2>;							// 00
	using tYear = type::tUIntFixedNoNull<4>;							// 1980
	using tLocalZoneHours = type::tInt<3>;								// 00 to ±13 hrs	(not supported)
	using tLocalZoneMinutes = type::tUIntFixed<2>;						// 00 to +59		(not supported)
	
	using tContentGGA = base::tContentGGA<tTime, tLatitude, tLongitude, tQuality, tSatQty, tHDOP, tAltitude, tGeoidSeparation, tDiffAge, tDiffStation>;
	using tContentGLL = base::tContentGLL7<tLatitude, tLongitude, tTime>;
	using tContentGSA = base::tContentGSA<tSatID, tHDOP>;
	using tContentGSV = generic::tContentGSV;
	using tContentRMC = base::tContentRMC12<tTime, tLatitude, tLongitude, tSpeed, tCourse, tDate>;
	using tContentVTG = base::tContentVTG9<tCourse, tSpeed>;
	using tContentZDA = base::tContentZDA<tTime, tDay, tMonth, tYear, tLocalZoneHours, tLocalZoneMinutes>;
}

namespace sirf_lr9548s
{
	using tTime = type::tTimeNoNull<3>;									// 000000.000
	using tDate = type::tDateNoNull;
	using tLatitude = type::tLatitudeNoNull<4>;							// 0000.0000,?
	using tLongitude = type::tLongitudeNoNull<4>;						// 00000.0000,?
	using tQuality = type::tQualityNoNull;								// 0
	using tSatQty = type::tUIntFixedNoNull<2>;							// 00
	using tSatID = type::tUIntFixed<2>;									// 00		Satellite ID (GPS: 1-32, SBAS 33-64 (33=PRN120), GLONASS: 65-96) 
	using tHDOP = type::tFloatPrecisionFixedNoNull<2, 1>;				// ?.0
	using tAltitude = type::tFloatPrecisionFixedUnitNoNull<5, 1>;		// ?.0,M		-14.2,M
	using tGeoidSeparation = type::tFloatPrecisionFixedUnitNoNull<4, 1>;// ?.0,M
	using tDiffAge = type::tFloatPrecisionFixedNoNull<3, 1>;			// ?.0
	using tDiffStation = type::tUIntFixedNoNull<4>;						// 0000
	using tSpeed = type::tFloatNoNull<4, 2>;							// ?.?
	using tCourse = type::tFloatPrecisionFixedNoNull<3, 2>;				// ?.00
	using tMode = type::tModeNoNull;									// A

	using tContentGGA = base::tContentGGA<tTime, tLatitude, tLongitude, tQuality, tSatQty, tHDOP, tAltitude, tGeoidSeparation, tDiffAge, tDiffStation>;
	using tContentGLL = base::tContentGLL7<tLatitude, tLongitude, tTime>;
	using tContentGSA = base::tContentGSA<tSatID, tHDOP>;
	using tContentGSV = generic::tContentGSV;
	using tContentRMC = base::tContentRMC13<tTime, tLatitude, tLongitude, tSpeed, tCourse, tDate, tMode>;
	using tContentVTG = base::tContentVTG10<tCourse, tSpeed, tMode>;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
