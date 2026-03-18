#pragma once

#include <string>

namespace dev
{

enum class tReceiverModel
{
	None,
	MTK_EB_800A,
	MTK_SC872_A,
	SiRF_GSU_7x,
	SiRF_LR9548S,
};

struct tDataSetHW
{
	std::string Manufacturer;
	std::string Model;
	std::string Firmware;
	std::string FirmwareID; // ID/CRC
	std::string Chip;
	//std::string ChipModel;
	//std::string Status;
	tReceiverModel ModelID;

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
