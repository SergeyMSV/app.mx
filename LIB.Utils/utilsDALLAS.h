///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsDALLAS
// 2024-01-30
// C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "utilsPortSerial.h"

#include <optional>

namespace utils
{
namespace dallas
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

using tBoardOneWire = utils::port::serial::tPortOneWireSync; // [TBD] put in libConfig

struct tID
{
	std::uint8_t Value[6]{};

	bool operator == (const tID&) const = default;
	bool operator < (const tID& id) const
	{
		return std::memcmp(this->Value, id.Value, sizeof(this->Value)) < 0;
	}
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
bool operator < (const tROM& a, const tROM& b);

tROM MakeROM(tFamilyCode familyCode, tID id);

struct tDsDS18B20
{
	tROM ROM;
	double Temperature;
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

	template<typename T>
	std::vector<tDsDS18B20> GetDsDS18B20(const T& devices)
	{
		using ValueType = typename T::value_type;
		static_assert(std::is_same_v<ValueType, tROM>, "T must store tROM elements");

		std::vector<tROM> romsDS18B20;
		for (auto& i : devices)
		{
			if (i.Field.FamilyCode == tFamilyCode::DS18B20)
				romsDS18B20.push_back(i);
		}
		return GetDsDS18B20Internal(romsDS18B20);
	}

	std::optional<tID> ReadKey(); // Only one device (key) can be connected to the bus.
	tCodeError WriteKey(const tID& value); // It writes ROM if only one device connected to the bus. In other words only one key must be connected to the bus and not original one.

private:
	std::optional<tROM> Search(tFamilyCode familyCode, int& lastDiscrepancy, int& lastFamilyDiscrepancy, bool& lastDeviceFlag);
	std::vector<tDsDS18B20> GetDsDS18B20Internal(const std::vector<tROM>& a_roms);
};

}
}