///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketNMEAPayloadPTWS
// 2020-02-11
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
namespace telit
{
///////////////////////////////////////////////////////////////////////////////////////////////////
	
//$PTWS,JAM,SIGNAL,VAL,INDEX,7,FREQ,1575.453369*60
//$PTWS,JAM,SIGNAL,VAL,INDEX,8,FREQ,1575.197876*67
//$PTWS,JAM,SIGNAL,VAL,INDEX,9,FREQ,1575.260742*65
//$PTWS,JAM,SIGNAL,VAL,INDEX,10,FREQ,1575.130615*58
//$PTWS,JAM,SIGNAL,VAL,INDEX,11,FREQ,1575.385498*52

struct tPayloadJAM_SIGNAL_VAL : public type::tTypeVerified
{
	using tIndex = type::tUInt<2>;
	using tFrequency = type::tFloatPrecisionFixed<4, 6>;

	tIndex Index;
	tFrequency Frequency;

	tPayloadJAM_SIGNAL_VAL() = default;
	tPayloadJAM_SIGNAL_VAL(std::uint8_t index, double frequency)
		:Index(index), Frequency(frequency)
	{
	}
	explicit tPayloadJAM_SIGNAL_VAL(const tPayloadCommon::value_type& val)
	{
		if (val.size() != 8 || val[0] != "PTWS" || val[1] != "JAM" || val[2] != "SIGNAL" || val[3] != "VAL" || val[4] != "INDEX" || val[6] != "FREQ")
		{
			SetVerified(false);
			return;
		}
		Index = tIndex(val[5]);
		Frequency = tFrequency(val[7]);
	}

	bool IsVerified() const { return type::tTypeVerified::IsVerified() && type::IsVerified(Index, Frequency); }

	tPayloadCommon::value_type GetPayload() const
	{
		tPayloadCommon::value_type Data;
		Data.push_back("PTWS");
		Data.push_back("JAM");
		Data.push_back("SIGNAL");
		Data.push_back("VAL");
		Data.push_back("INDEX");
		Data.push_back(Index.ToString());
		Data.push_back("FREQ");
		Data.push_back(Frequency.ToString());
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tPayloadVERSION_GET
{
	tPayloadVERSION_GET() = default;
	//explicit tPayloadVERSION_GET(const tPayloadCommon::value_type& val)
	//{
	//	if (val.size() != 3 || val[0] != "PTWS" || val[1] != "VERSION" || val[2] != "GET")
	//		SetVerified(false);
	//}

	bool IsVerified() const { return true; }

	tPayloadCommon::value_type GetPayload() const
	{
		tPayloadCommon::value_type Data;

		Data.push_back("PTWS");
		Data.push_back("VERSION");
		Data.push_back("GET");

		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tPayloadVERSION_VAL : public type::tTypeVerified
{
	using tVersion = std::string;

	tVersion Version;

	tPayloadVERSION_VAL() = default;
	explicit tPayloadVERSION_VAL(tVersion version)
		:Version(version)
	{}
	explicit tPayloadVERSION_VAL(const tPayloadCommon::value_type& val)
	{
		if (val.size() != 4 || val[0] != "PTWS" || val[1] != "VERSION" || val[2] != "VAL")
		{
			SetVerified(false);
			return;
		}
		Version = val[3];
	}

	bool IsVerified() const { return type::tTypeVerified::IsVerified() && !Version.empty(); }

	tPayloadCommon::value_type GetPayload() const
	{
		tPayloadCommon::value_type Data;
		Data.push_back("PTWS");
		Data.push_back("VERSION");
		Data.push_back("VAL");
		Data.push_back(Version);
		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
}
