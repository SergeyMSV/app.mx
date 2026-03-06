#pragma once

#include <string>

namespace dev
{

struct tDataSetHW
{
	std::string Manufacturer;
	std::string Model;
	std::string Firmware;
	std::string FirmwareID; // ID/CRC
	std::string Chip;
	//std::string ChipModel;
	//std::string Status;

	std::string ToString() const
	{
		std::string Str;
		Str +=   "Manufacturer: " + Manufacturer;
		Str += "\nModel:        " + Model;
		Str += "\nFirmware:     " + Firmware;
		Str += "\nFirmware ID:  " + FirmwareID;
		Str += "\nChip:         " + Chip;
		//Str += "\nStatus:       " + Status;
		return Str;
	}
};

}
