///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketNMEAPayloadPSRF
// 2026-02-20
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
namespace sirf
{
///////////////////////////////////////////////////////////////////////////////////////////////////
using tContentP = base::tContentP<4, 3>;
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPSRF : public tContentP
{
	tContentPSRF() = default;
	explicit tContentPSRF(const std::vector<std::string>& val)
		:tContentP(val)
	{
		SetVerified(Value.size() >= 2 && Value[0] == GetID());
	}

	static const char* GetID() { return "PSRF"; }
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPSRF_TXT : public tContentPSRF
{
	tContentPSRF_TXT() = default;
	explicit tContentPSRF_TXT(const std::vector<std::string>& val)
		:tContentPSRF(val)
	{
		if (Value.size() < 3 || Value[1] != "TXT")
			SetVerified(false);
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPSRF_SetSerialPort : public tContentPSRF // PSRF100
{
	enum class tProtocol { Binary, NMEA, };

	tProtocol Protocol = tProtocol::Binary;
	std::uint32_t Baudrate = 0;

	explicit tContentPSRF_SetSerialPort(const std::vector<std::string>& val)
		:tContentPSRF(val)
	{
		if (Value.size() != 7 || Value[1] != GetID())
		{
			SetVerified(false);
			return;
		}

		if (Value[2] != "0" && Value[2] == "1")
		{
			SetVerified(false);
			return;
		}
		Protocol = static_cast<tProtocol>(std::atoi(Value[2].c_str()));
		
		const std::uint32_t Br = static_cast<std::uint32_t>(std::atoi(Value[3].c_str()));
		if (!CheckBaudrate(Br))
		{
			SetVerified(false);
			return;
		}
		Baudrate = Br;
	}
	explicit tContentPSRF_SetSerialPort(const std::uint32_t baudrate)
		:Protocol(tProtocol::NMEA), Baudrate(baudrate)
	{
		SetVerified(CheckBaudrate(Baudrate));
	}

	static const char* GetID() { return "100"; }

	std::vector<std::string> GetPayload() const
	{
		if (!IsVerified())
			return {};
		std::vector<std::string> Data;
		Data.push_back(std::string(tContentPSRF::GetID()) + GetID());
		Data.push_back(std::to_string(static_cast<int>(Protocol)));
		Data.push_back(std::to_string(static_cast<int>(Baudrate)));
		Data.push_back("8"); // DataBits	8,7 (SiRF protocol is only valid for 8 data bits, 1stop bit, and no parity)
		Data.push_back("1"); // StopBits	0,1
		Data.push_back("0"); // Parity		0=None, 1=Odd, 2=Even
		return Data;
	}

private:
	bool CheckBaudrate(std::uint32_t br) const
	{
		return Contains<std::uint32_t>({ 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 }, br);
		//return Contains<std::uint32_t>({ 4800, 9600, 19200, 38400 }, br); // Legacy
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// legacy, LEADTEK compatible
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace sirf_legacy
{
///////////////////////////////////////////////////////////////////////////////////////////////////
using tContentP = base::tContentP<4, 3, 50>;
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPSRF : public tContentP
{
	tContentPSRF() = default;
	explicit tContentPSRF(const std::vector<std::string>& val)
		:tContentP(val)
	{
		if (Value.size() < 2 || Value[0] != GetID())
			SetVerified(false);
	}
	explicit tContentPSRF(const tContentP& val)
		:tContentP(val)
	{
		if (Value.size() < 2 || Value[0] != GetID())
			SetVerified(false);
	}

	static const char* GetID() { return "PSRF"; }
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tContentPSRF_TXT : public tContentPSRF
{
	tContentPSRF_TXT() = default;
	explicit tContentPSRF_TXT(const std::vector<std::string>& val)
		:tContentPSRF(val)
	{
		if (Value.size() < 3 || Value[1] != "TXT")
			SetVerified(false);
	}
};
///////////////////////////////////////////////////////////////////////////////////////////////////
}

}
}
}
