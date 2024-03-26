///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsDALLAS
// 2024-01-30
// Standard ISO/IEC 114882, C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsPortSerial.h"

#include <optional>

namespace utils
{
namespace DALLAS
{

enum class tFamilyCode : std::uint8_t
{
	None,
	DS1990A = 0x01, // iButton
	DS1991  = 0x02, // + (protected NV RAM bits)
	DS1992  = 0x08,
	DS1993  = 0x06,
	DS1994  = 0x04, // + (RTC, interval timer, cycle counter)
	DS1995  = 0x0A,
	DS1996  = 0x0C,
	DS1982  = 0x09,
	DS1985  = 0x0B,
	DS1986  = 0x0F,
	DS1920  = 0x10,	// + (temperature)
	DS18B20 = 0x28, // Thermometer
};

union tTemperature
{
	struct
	{
		std::uint16_t Degree_00625 : 1;
		std::uint16_t Degree_0125  : 1;
		std::uint16_t Degree_025   : 1;
		std::uint16_t Degree_05    : 1;
		std::uint16_t Degree       : 7;
		std::uint16_t Sign         : 5; // if Sign > 0 then Value is negative
	}Field;
	std::uint16_t Value = 0;
};

double ToDouble(tTemperature val);

using tBoardOneWire = utils::port_serial::tPortOneWireSync; // [TBD] put in libConfig

struct tID
{
	std::uint8_t Value[6]{};

	bool operator == (const tID&) const = default;
};

union tROM
{
	struct
	{
		tFamilyCode FamilyCode;
		tID ID;
		std::uint8_t CRC;
	}Field;
	std::uint8_t Value[8]{};
};

bool operator == (const tROM& a, const tROM& b);

tROM MakeROM(tFamilyCode familyCode, tID id);

struct tThermal
{
	tROM ROM;
	tTemperature Temperature;
};

enum class tCodeError
{
	Success,
	Connection,
	SameKeyID,
	NotWritten,
	WrongKeyID,
};

class tDALLAS : public tBoardOneWire
{
	static constexpr int m_DevQtyMAX = 10; // [#] not more than 10 devices on the bus

public:
	tDALLAS(boost::asio::io_context& ioc, const std::string& id, tSpeed speed = tSpeed::Norm)
		:tPortOneWireSync(ioc, id, speed)
	{}
	~tDALLAS() {}

	std::vector<tROM> Search(tFamilyCode familyCode);
	std::vector<tROM> Search();

	std::vector<tThermal> GetTemperature(const std::vector<tROM>& devices);

	std::optional<tID> ReadKey(); // Only one device (key) can be connected to the bus.
	tCodeError WriteKey(const tID& value); // It writes ROM if only one device connected to the bus. In other words only one key must be connected to the bus and not original one.

private:
	std::optional<tROM> Search(tFamilyCode familyCode, int& lastDiscrepancy, int& lastFamilyDiscrepancy, bool& lastDeviceFlag);
};

}
}