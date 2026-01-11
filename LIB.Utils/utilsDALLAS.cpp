#include "utilsCRC.h"
#include "utilsDALLAS.h"

#include <chrono>
#include <thread>

namespace utils
{
namespace dallas
{

constexpr std::uint8_t Cmd_ReadROM        = 0x33;
constexpr std::uint8_t Cmd_ConvertT       = 0x44;
constexpr std::uint8_t Cmd_MatchROM       = 0x55;
constexpr std::uint8_t Cmd_ReadScratchpad = 0xBE;
constexpr std::uint8_t Cmd_SkipROM        = 0xCC;
constexpr std::uint8_t Cmd_SearchROM      = 0xF0;

constexpr std::uint8_t Cmd_RW1990_WriteSession = 0xD1; // Open/Close write session
constexpr std::uint8_t Cmd_RW1990_Write        = 0xD5;

bool operator == (const tROM& a, const tROM& b)
{
	return !memcmp(a.Value, b.Value, sizeof(a.Value));
}

double ParseTemperature(const std::vector<std::uint8_t>& scratchPad)
{
	if (scratchPad.size() != 9) // [TBD] check 9 bytes and CRC
		return -999999; // [#]
	std::uint8_t CRC = utils::crc::CRC08_DALLAS(scratchPad.data(), scratchPad.size() - 1);
	if (scratchPad[scratchPad.size() - 1] != CRC)
		return -999999; // [#]

	enum class tResolution : std::uint8_t
	{
		_09_bit,
		_10_bit,
		_11_bit,
		_12_bit,
	};

	tResolution Resol = static_cast<tResolution>((scratchPad[4] >> 5) & 0x03);

	std::int16_t TempRaw = (std::int16_t)((scratchPad[1] << 8) | scratchPad[0]);
	std::int16_t TempSign = (std::int16_t)(TempRaw >> 4);
	double Temp = TempSign;

	if(Resol == tResolution::_12_bit)
		Temp += (TempRaw & 0x01) == 0 ? 0 : 0.0625;

	if (Resol >= tResolution::_11_bit)
		Temp += (TempRaw & 0x02) == 0 ? 0 : 0.125;

	if (Resol >= tResolution::_10_bit)
		Temp += (TempRaw & 0x04) == 0 ? 0 : 0.25;

	Temp += (TempRaw & 0x08) == 0 ? 0 : 0.5;
	return Temp;
}

static std::uint8_t GetROMCRC(const tROM& rom)
{
	return utils::crc::CRC08_DALLAS(rom.Value, sizeof(rom.Value) - 1);
}

static bool VerifyROM(const tROM& rom)
{
	return GetROMCRC(rom) == rom.Field.CRC;
}

static bool VerifyROM(const std::vector<std::uint8_t>& rom)
{
	if (rom.size() != 8)
		return false;
	std::uint8_t CRC = utils::crc::CRC08_DALLAS(rom.data(), rom.size() - 1);
	return CRC == rom[rom.size() - 1];
}

static tROM ToROM(const std::vector<std::uint8_t>& romRaw)
{
	if (!VerifyROM(romRaw))
		return {};
	tROM ROM{};
	std::copy(romRaw.begin(), romRaw.end(), reinterpret_cast<std::uint8_t*>(&ROM.Value));
	return ROM;
}

tROM MakeROM(tFamilyCode familyCode, tID id)
{
	tROM ROM{};
	ROM.Field.FamilyCode = familyCode;
	ROM.Field.ID = id;
	ROM.Field.CRC = GetROMCRC(ROM);
	return ROM;
}

std::vector<tROM> tDALLAS::Search(tFamilyCode familyCode)
{
	std::vector<tROM> ROMs;
	int LastDiscrepancy = 0;
	int LastFamilyDiscrepancy = 0;
	if (familyCode != tFamilyCode::None)
		LastDiscrepancy = 64;
	bool LastDeviceFlag = false;
	for (int i = 0; i < m_DevQtyMAX; ++i)
	{
		std::optional<tROM> ROM = Search(familyCode, LastDiscrepancy, LastFamilyDiscrepancy, LastDeviceFlag);
		if (!ROM.has_value())
			break;
		if (familyCode != tFamilyCode::None && ROM->Field.FamilyCode != familyCode)
			break;
		ROMs.push_back(*ROM);
	}
	return ROMs;
}

std::vector<tROM> tDALLAS::Search()
{
	return Search(tFamilyCode::None);
}

std::vector<tDsDS18B20> tDALLAS::GetDsDS18B20(const std::vector<tROM>& devices)
{
	std::vector<tROM> ThermoROMs;
	for (auto& i : devices)
	{
		if (i.Field.FamilyCode == tFamilyCode::DS18B20)
			ThermoROMs.push_back(i);
	}

	if (ThermoROMs.empty())
		return {};

	if (Reset() != tStatus::Success)
		return {};

	Transaction({ Cmd_SkipROM }, 0);
	Transaction({ Cmd_ConvertT }, 0);

	if (ThermoROMs.size() == 1) // [TBD] It's also not parasite-powered.
	{
		for (int i = 0; i < 100; ++i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 800ms but 750ms enough
			std::vector<std::uint8_t> Rsp = Transaction({ Cmd_ConvertT }, 1); // Initiates temperature conversion (not applicable for parasite-powered DS18B20s)
			if (Rsp.size() == 1 && Rsp[0] != 0x00)
				break;
		}
	}
	else // Applicable for parasite-powered DS18B20s.
	{		
		std::this_thread::sleep_for(std::chrono::milliseconds(800)); // 800ms but 750ms enough
	}

	std::vector<tDsDS18B20> ThermalSens;
	for (auto& i : ThermoROMs)
	{
		if (Reset() != tStatus::Success)
			break;

		std::vector<std::uint8_t> CmdMatchROM;
		CmdMatchROM = { Cmd_MatchROM };
		CmdMatchROM.insert(CmdMatchROM.end(), static_cast<std::uint8_t*>(i.Value), static_cast<std::uint8_t*>(i.Value) + sizeof(i.Value));
		Transaction(CmdMatchROM, 0);
		std::vector<std::uint8_t> Rsp = Transaction({ Cmd_ReadScratchpad }, 9); // Reads the entire scratchpad including the CRC byte (DS18B20 transmits up to 9 data bytes to master)
		if (Rsp.size() != 9)
			continue;
		std::uint8_t CRC = utils::crc::CRC08_DALLAS(Rsp.data(), Rsp.size() - 1);
		if (Rsp[Rsp.size() - 1] != CRC)
			continue;
		tDsDS18B20 Thermal{};
		Thermal.ROM = i;
		Thermal.Temperature = ParseTemperature(Rsp);
		ThermalSens.push_back(Thermal);
	}

	return ThermalSens;
}

std::optional<tID> tDALLAS::ReadKey()
{
	if (Reset() != tStatus::Success)
		return {};
	tROM ROM = ToROM(Transaction({ Cmd_ReadROM }, 8));
	if (!VerifyROM(ROM) || ROM.Field.FamilyCode != tFamilyCode::DS1990A)
		return {};
	return ROM.Field.ID;
}

tCodeError tDALLAS::WriteKey(const tID& id)
{
	tROM ROM{};
	ROM.Field.FamilyCode = tFamilyCode::DS1990A;
	ROM.Field.ID = id;
	ROM.Field.CRC = GetROMCRC(ROM);

	if (Reset() != tStatus::Success)
		return tCodeError::Connection;
	tROM ROMConnected = ToROM(Transaction({ Cmd_ReadROM }, 8));
	if (ROM == ROMConnected)
		return tCodeError::SameKeyID;

	if (Reset() != tStatus::Success)
		return tCodeError::Connection;
	Transaction({ Cmd_RW1990_WriteSession }, 0); // Open write session

	if (Reset() != tStatus::Success)
		return tCodeError::Connection;
	Transaction({ Cmd_RW1990_Write }, 0);

	const std::vector<std::uint8_t> ID_Bits = ByteToBit(std::vector(std::begin(ROM.Value), std::end(ROM.Value)));
	for (auto& i : ID_Bits)
	{
		SendBit(!i);
		std::this_thread::sleep_for(std::chrono::milliseconds(20)); // it must be more than 10 ms
	}

	if (Reset() != tStatus::Success)
		return tCodeError::Connection;
	Transaction({ Cmd_RW1990_WriteSession }, 0); // Close write session

	if (Reset() != tStatus::Success)
		return tCodeError::Connection;
	tROM ROMWritten = ToROM(Transaction({ Cmd_ReadROM }, 8));
	if (ROMWritten == ROM)
		return tCodeError::Success;
	if (ROMWritten == ROMConnected)
		return tCodeError::NotWritten;
	return tCodeError::WrongKeyID;
}

// APPLICATION NOTE 187 "1 - Wire Search Algorithm" Mar 28, 2002
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing search state.
std::optional<tROM> tDALLAS::Search(tFamilyCode familyCode, int& lastDiscrepancy, int& lastFamilyDiscrepancy, bool& lastDeviceFlag)
{
	int id_bit_number = 1;
	int last_zero = 0;
	int rom_byte_number = 0;
	bool search_result = false;
	unsigned char rom_byte_mask = 1;
	bool search_direction = false;

	tROM ROM{};
	ROM.Field.FamilyCode = familyCode;

	std::uint8_t ROM_CRC = 0;

	// if the last call was not the last one
	if (!lastDeviceFlag)
	{
		if (Reset() != tBoardOneWire::tStatus::Success)
			return {};

		Transaction({ Cmd_SearchROM }, 0);

		do
		{
			std::vector<bool> RspBit = ReceiveBits(2);
			if (RspBit.size() != 2)
				return {};

			bool id_bit = RspBit[0];
			bool cmp_id_bit = RspBit[1];

			if (id_bit && cmp_id_bit) // check for no devices on 1-wire
				break;

			// all devices coupled have 0 or 1
			if (id_bit != cmp_id_bit)
			{
				search_direction = id_bit; // bit write value for search
			}
			else
			{
				// if this discrepancy if before the Last Discrepancy
				// on a previous next then pick the same as last time
				if (id_bit_number < lastDiscrepancy)
					search_direction = (ROM.Value[rom_byte_number] & rom_byte_mask) > 0;
				else
					// if equal to last pick 1, if not then pick 0
					search_direction = id_bit_number == lastDiscrepancy;

				// if 0 was picked then record its position in LastZero
				if (!search_direction)
				{
					last_zero = id_bit_number;
					// check for Last discrepancy in family
					if (last_zero < 9)
						lastFamilyDiscrepancy = last_zero;
				}
			}

			// set or clear the bit in the ROM byte rom_byte_number with mask rom_byte_mask
			if (search_direction)
				ROM.Value[rom_byte_number] |= rom_byte_mask;
			else
				ROM.Value[rom_byte_number] &= ~rom_byte_mask;

			SendBit(search_direction); // serial number search direction write bit

			// increment the byte counter id_bit_number and shift the mask rom_byte_mask
			id_bit_number++;
			rom_byte_mask <<= 1;
			// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
			if (rom_byte_mask == 0)
			{
				ROM_CRC = utils::crc::CRC08_DALLAS(ROM.Value[rom_byte_number], ROM_CRC);
				rom_byte_number++;
				rom_byte_mask = 1;
			}
		} while (rom_byte_number < 8); // loop until through all ROM bytes 0-7
		// if the search was successful then
		if (!(id_bit_number < 65 || ROM_CRC))
		{
			// search successful so set LastDiscrepancy, LastDeviceFlag, search_result
			lastDiscrepancy = last_zero;
			// check for last device
			if (!lastDiscrepancy)
				lastDeviceFlag = true;
			search_result = true;
		}
	}
	// if no device found then reset counters so next 'search' will be like a first
	if (!search_result || !ROM.Value[0])
		return {};

	return ROM;
}

}
}
