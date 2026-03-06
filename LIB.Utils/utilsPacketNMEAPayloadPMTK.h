///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketNMEAPayloadPMTK
// 2020-02-07
// C++17
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsPacketNMEA.h"
#include "utilsPacketNMEAType.h"
#include "utilsPacketNMEAPayloadP.h"

namespace utils
{
namespace packet
{
namespace nmea
{
namespace mtk
{
///////////////////////////////////////////////////////////////////////////////////////////////////
using tContentP = base::tContentP<4, 3>;
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPMTK : public tContentP
{
	tContentPMTK() = default;
	explicit tContentPMTK(const std::vector<std::string>& val)
		:tContentP(val)
	{
		Verify();
	}
	explicit tContentPMTK(const tContentP& val)
	{
		Verify();
	}

private:
	void Verify()
	{
		SetVerified(Value.size() >= 2 && Value[0] == "PMTK");
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPMTK_TEST : public type::tTypeVerified
{
	tContentPMTK_TEST() = default;
	explicit tContentPMTK_TEST(const std::vector<std::string>& val)
	{
		Verify(tContentPMTK(val));
	}
	explicit tContentPMTK_TEST(const tContentPMTK& val)
	{
		Verify(val);
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;
		Data.push_back("PMTK000");
		return Data;
	}

private:
	void Verify(const tContentPMTK& parsed)
	{
		SetVerified(parsed.Value.size() >= 2 && parsed.Value[0] == "PMTK");
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentACK : public type::tTypeVerified
{
	enum class tStatus : std::uint8_t
	{
		InvalidCommand,			// 0 = Invalid command / packet
		UnsupportedCommand,		// 1 = Unsupported command / packet type
		Failed,					// 2 = Valid command / packet, but action failed.
		Succeeded,				// 3 = Valid command / packet, and action succeeded
	};

	using tCmd = type::tUIntFixed<3>;
	using tFlag = type::tUIntFixed<1>;

	tCmd Cmd;
	tFlag Status;

	tContentACK() = default;
	explicit tContentACK(const std::vector<std::string>& val)
	{
		SetValues(tContentPMTK(val));
	}
	explicit tContentACK(const tContentPMTK& val)
	{
		SetValues(val);
	}

	bool IsVerified() const { return type::tTypeVerified::IsVerified() && type::IsVerified(Cmd, Status); }

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;
		Data.push_back("PMTK001");
		Data.push_back(Cmd.ToString());
		Data.push_back(Status.ToString());
		return Data;
	}

private:
	void SetValues(const tContentPMTK& parsed)
	{
		if (!parsed.IsVerified() || parsed.Value.size() != 4 || parsed.Value[1] != "001")
		{
			SetVerified(false);
			return;
		}
		Cmd = tCmd(parsed.Value[2]);
		Status = tFlag(parsed.Value[3]);
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
//struct tContentACK_GNSS_SEARCH_MODE : public type::tTypeVerified
//{
//	enum class tStatus : std::uint8_t
//	{
//		InvalidCommand,			// 0 = Invalid command / packet
//		UnsupportedCommand,		// 1 = Unsupported command / packet type
//		Failed,					// 2 = Valid command / packet, but action failed.
//		Succeeded,				// 3 = Valid command / packet, and action succeeded
//	};
//
//	using tCmd = type::tUIntFixed<3>;
//	using tFlag = type::tUIntFixed<1>;
//
//	tCmd Cmd;
//	tFlag Status;
//
//	tContentACK_GNSS_SEARCH_MODE() = default;
//	explicit tContentACK_GNSS_SEARCH_MODE(const std::vector<std::string>& val)
//	{
//		tContentPMTK Parsed(val);
//		if (!Parsed.IsVerified() || Parsed.Value.size() != 4 || Parsed.Value[1] != "001")
//		{
//			SetVerified(false);
//			return;
//		}
//		Cmd = tCmd(val[1]);
//		Status = tFlag(val[2]);
//
//	}
//
//	bool IsVerified() const { return type::tTypeVerified::IsVerified() && type::IsVerified(Cmd, Status); }
//
//	std::vector<std::string> GetPayload() const
//	{
//		std::vector<std::string> Data;
//		Data.push_back("PMTK001");
//		Data.push_back(Cmd.ToString());
//		Data.push_back(Status.ToString());
//		return Data;
//	}
//
//private:
//	void SetValues(const tContentPMTK& parsed)
//	{
//		if (!parsed.IsVerified() || parsed.Value.size() != 4 || parsed.Value[1] != "001")
//		{
//			SetVerified(false);
//			return;
//		}
//		Cmd = tCmd(parsed.Value[2]);
//		Status = tFlag(parsed.Value[3]);
//	}
//};

/*

template <int CmdID>
struct tContentPMTK001 : public hidden::tContentPMTK001
{
	explicit tContentPMTK001(const std::vector<std::string>& val)
		:hidden::tContentPMTK001(val)
	{ }

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 3 && hidden::tContentPMTK001::Try(val);
	}
};

template <>
struct tContentPMTK001<355> : public hidden::tContentPMTK001
{
	typedef Type::tUInt<std::uint8_t, 0> status_type;

	status_type GPS;
	status_type GLONASS;
	status_type BEIDOU;

	explicit tContentPMTK001(const std::vector<std::string>& val)
		:hidden::tContentPMTK001(val)
	{
		if (Try(val))
		{
			GPS = status_type(val[3]);
			GLONASS = status_type(val[4]);
			BEIDOU = status_type(val[5]);
		}
	}

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 7 && hidden::tContentPMTK001::Try(val);
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data = hidden::tContentPMTK001::GetPayload();

		Data.push_back(GPS.ToString());
		Data.push_back(GLONASS.ToString());
		Data.push_back(BEIDOU.ToString());
		Data.push_back("0");

		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPMTK010
{
	enum class tStatus : std::uint8_t
	{
		UNKNOWN = 0,
		STARTUP,
		EPO,//Notification for the host aiding Extended Prediction Orbit (EPO)
		NORMAL,//Notification for the transition to Normal mode is successfully done
		UNINITIALIZED = 0xFF,//Notification for the transition to Normal mode is successfully done
	};

	typedef Type::tUInt<tStatus, 3> status_type;

	status_type Status;

	tContentPMTK010() = default;
	explicit tContentPMTK010(const std::vector<std::string>& val)
	{
		if (Try(val))
		{
			Status = status_type::Parse(val[1]);
		}
	}

	static const char* GetID() { return "PMTK010"; }

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 2 && !std::strcmp(val[0].c_str(), GetID());
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;

		Data.push_back(GetID());
		Data.push_back(Status.ToString());

		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPMTK011
{
	typedef std::string message_type;

	message_type Message;

	tContentPMTK011() = default;
	explicit tContentPMTK011(const std::vector<std::string>& val)
	{
		if (Try(val))
		{
			Message = val[1];
		}
	}

	static const char* GetID() { return "PMTK011"; }

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 2 && !std::strcmp(val[0].c_str(), GetID());
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;

		Data.push_back(GetID());
		Data.push_back(Message);

		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hidden
{

enum class tPayloadPMTK10xResetState : std::uint8_t
{
	Hot = 1,
	Warm,
	Cold,
	Full
};

template <tPayloadPMTK10xResetState State>
struct tContentPMTK10xReset
{
	tContentPMTK10xReset() = default;
	explicit tContentPMTK10xReset(const std::vector<std::string>& val) {}

	static const char* GetID()
	{
		switch (State)
		{
		case tPayloadPMTK10xResetState::Hot: return "PMTK101";
		case tPayloadPMTK10xResetState::Warm: return "PMTK102";
		case tPayloadPMTK10xResetState::Cold: return "PMTK103";
		case tPayloadPMTK10xResetState::Full: return "PMTK104";
		}

		assert(false);

		return "";
	}

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 1 && !std::strcmp(val[0].c_str(), GetID());
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;

		Data.push_back(GetID());

		return Data;
	}
};

}
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef hidden::tContentPMTK10xReset<hidden::tPayloadPMTK10xResetState::Hot> tPayloadPMTK101ResetHot;
typedef hidden::tContentPMTK10xReset<hidden::tPayloadPMTK10xResetState::Warm> tPayloadPMTK102ResetWarm;
typedef hidden::tContentPMTK10xReset<hidden::tPayloadPMTK10xResetState::Cold> tPayloadPMTK103ResetCold;
typedef hidden::tContentPMTK10xReset<hidden::tPayloadPMTK10xResetState::Full> tPayloadPMTK104ResetFull;
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPMTK314R2
{
	typedef Type::tUInt<std::uint8_t, 0> status_type;

	status_type GLL;
	status_type RMC;
	status_type VTG;
	status_type GGA;
	status_type GSA;
	status_type GSV;
	status_type ZDA;

	tContentPMTK314R2() = default;
	tContentPMTK314R2(std::uint8_t gll, std::uint8_t rmc, std::uint8_t vtg, std::uint8_t gga, std::uint8_t gsa, std::uint8_t gsv, std::uint8_t zda)
		:GLL(gll), RMC(rmc), VTG(vtg), GGA(gga), GSA(gsa), GSV(gsv), ZDA(zda)
	{ }
	explicit tContentPMTK314R2(const std::vector<std::string>& val)
	{
		if (Try(val))
		{
			GLL = status_type(val[1]);
			RMC = status_type(val[2]);
			VTG = status_type(val[3]);
			GGA = status_type(val[4]);
			GSA = status_type(val[5]);
			GSV = status_type(val[6]);
			ZDA = status_type(val[18]);
		}
	}

	static const char* GetID() { return "PMTK314"; }

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 20 && !std::strcmp(val[0].c_str(), GetID());
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;

		Data.push_back(GetID());
		Data.push_back(GLL.ToString());
		Data.push_back(RMC.ToString());
		Data.push_back(VTG.ToString());
		Data.push_back(GGA.ToString());
		Data.push_back(GSA.ToString());
		Data.push_back(GSV.ToString());
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back(ZDA.ToString());
		Data.push_back("0");

		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPMTK314R3
{
	typedef Type::tUInt<std::uint8_t, 0> status_type;

	status_type GLL;
	status_type RMC;
	status_type VTG;
	status_type GGA;
	status_type GSA;
	status_type GSV;
	status_type GRS;
	status_type GST;
	status_type ZDA;
	status_type MCHN;
	status_type DTM;
	status_type GBS;

	tContentPMTK314R3() = default;
	tContentPMTK314R3(std::uint8_t gll, std::uint8_t rmc, std::uint8_t vtg, std::uint8_t gga, std::uint8_t gsa, std::uint8_t gsv, std::uint8_t grs, std::uint8_t gst, std::uint8_t zda, std::uint8_t mchn, std::uint8_t dtm, std::uint8_t gbs)
		:GLL(gll), RMC(rmc), VTG(vtg), GGA(gga), GSA(gsa), GSV(gsv), GRS(grs), GST(gst), ZDA(zda), MCHN(mchn), DTM(dtm), GBS(gbs)
	{ }
	tContentPMTK314R3(std::uint8_t gll, std::uint8_t rmc, std::uint8_t vtg, std::uint8_t gga, std::uint8_t gsa, std::uint8_t gsv, std::uint8_t zda)
		:tContentPMTK314R3(gll, rmc, vtg, gga, gsa, gsv, 0, 0, zda, 0, 0, 0)
	{ }	
	explicit tContentPMTK314R3(const std::vector<std::string>& val)
	{
		if (Try(val))
		{
			GLL = status_type(val[1]);
			RMC = status_type(val[2]);
			VTG = status_type(val[3]);
			GGA = status_type(val[4]);
			GSA = status_type(val[5]);
			GSV = status_type(val[6]);
			GRS = status_type(val[7]);
			GST = status_type(val[8]);
			ZDA = status_type(val[18]);
			MCHN = status_type(val[19]);
			DTM = status_type(val[20]);
			GBS = status_type(val[21]);
		}
	}

	static const char* GetID() { return "PMTK314"; }

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 22 && !std::strcmp(val[0].c_str(), GetID());
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;

		Data.push_back(GetID());
		Data.push_back(GLL.ToString());
		Data.push_back(RMC.ToString());
		Data.push_back(VTG.ToString());
		Data.push_back(GGA.ToString());
		Data.push_back(GSA.ToString());
		Data.push_back(GSV.ToString());
		Data.push_back(GRS.ToString());
		Data.push_back(GST.ToString());
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back("0");
		Data.push_back(ZDA.ToString());
		Data.push_back(MCHN.ToString());
		Data.push_back(DTM.ToString());
		Data.push_back(GBS.ToString());

		return Data;
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPMTK353
{
	typedef Type::tUInt<std::uint8_t, 0> status_type;

	status_type GPS;
	status_type GLONASS;

	tContentPMTK353() = default;
	tContentPMTK353(bool gps, bool glonass)
		:GPS(gps ? 1 : 0), GLONASS(glonass ? 1 : 0)
	{ }
	explicit tContentPMTK353(const std::vector<std::string>& val)
	{
		if (Try(val))
		{
			GPS = status_type(val[1]);
			GLONASS = status_type(val[2]);
		}
	}

	static const char* GetID() { return "PMTK353"; }

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 3 && !std::strcmp(val[0].c_str(), GetID());
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;

		Data.push_back(GetID());
		Data.push_back(GPS.ToString());
		Data.push_back(GLONASS.ToString());

		return Data;
	}

	Type::tGNSS_State GetState()
	{
		std::uint8_t State = 0;

		if (GPS.Value) State |= 1;
		if (GLONASS.Value) State |= (1 << 1);

		return static_cast<Type::tGNSS_State>(State);
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPMTK705
{
	typedef std::string release_type;
	typedef std::string build_type;
	typedef std::string internal_use_1_type;
	typedef std::string internal_use_2_type;

	release_type Release;
	build_type Build;
	internal_use_1_type InternalUse1;
	internal_use_2_type InternalUse2;

	tContentPMTK705() = default;
	tContentPMTK705(const std::string& release, const std::string& revision, const std::string& internalUse1, const std::string& internalUse2)
		:Release(release), Build(revision), InternalUse1(internalUse1), InternalUse2(internalUse2)
	{ }
	explicit tContentPMTK705(const std::vector<std::string>& val)
	{
		if (Try(val))
		{
			Release = val[1];
			Build = val[2];
			InternalUse1 = val[3];
			InternalUse2 = val[4];
		}
	}

	static const char* GetID() { return "PMTK705"; }

	static bool Try(const std::vector<std::string>& val)
	{
		return val.size() == 5 && !std::strcmp(val[0].c_str(), GetID());
	}

	std::vector<std::string> GetPayload() const
	{
		std::vector<std::string> Data;

		Data.push_back(GetID());
		Data.push_back(Release);
		Data.push_back(Build);
		Data.push_back(InternalUse1);
		Data.push_back(InternalUse2);

		return Data;
	}
};*/
///////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
}
