#include "devStateInit.h"

#include <utilsPacketNMEAPayload.h>
#include <utilsPacketNMEAPayloadPSRF.h>
#include <utilsString.h>

namespace dev
{
namespace state
{

using namespace utils::packet::nmea;

std::optional<tDataSetHW> TrySiRF_Parse_Legacy(const std::vector<sirf_legacy::tContentPSRF_TXT>& contents, const tDataSetHW& dsHW)
{
	if (contents.empty())
		return {};
	tDataSetHW DataSetHW = dsHW;
	for (auto& i : contents)
	{
		if (i.Value.size() != 3)
			continue;
		if (utils::string::Contains(i.Value[2], "Version ")) // $PSRFTXTVersion ...*XX
		{
			DataSetHW.Manufacturer = "LEADTEK";
			std::vector<std::string_view> Strs = utils::string::SplitTrimString(contents[0].Value[2], ' ');
			if (Strs.size() == 2)
				DataSetHW.Firmware = Strs[1];
			DataSetHW.Model = "LR9548S";
		}
	}
	if (DataSetHW.Model.empty())
		return {};
	return DataSetHW;
}

std::optional<tDataSetHW> TrySiRF_Parse(const std::vector<sirf_legacy::tContentPSRF_TXT>& contents, const tDataSetHW& dsHW)
{
	if (contents.empty())
		return {};
	tDataSetHW DataSetHW = dsHW;
	DataSetHW.Chip = "SiRF";
	for (auto& i : contents)
	{
		if (i.Value.size() < 3)
			continue;
		std::vector<std::string_view> Strs = utils::string::SplitTrimString(i.Value[2], ':');
		if (Strs.size() < 2)
			continue;
		if (utils::string::Contains(Strs[0], "GSU"))
		{
			DataSetHW.Model = Strs[0];
			DataSetHW.Manufacturer = Strs[1];
			if (i.Value.size() == 4)
				DataSetHW.Manufacturer += "," + i.Value[3];
		}
		else if (Strs[0] == "Firmware Checksum")
		{
			DataSetHW.FirmwareID = Strs[1];
		}
	}
	if (DataSetHW.Model.empty())
		return {};
	return DataSetHW;
}

std::optional<std::pair<tDataSetHW, tStatus>> TrySiRF(tPortUART& port, const tDataSetConfig& dsConfig)
{
	port.Send("$PSRF101,0,0,0,0,0,0,12,1*15\xd\xa"); // Hot Restart
	//port.Send("$PSRF101,0,0,0,0,0,0,12,2*16\xd\xa"); // Warm Restart
	//port.Send("$PSRF101,0,0,0,0,0,0,12,4*10\xd\xa"); // Cold Restart
	//port.Send("$PSRF101,0,0,0,0,0,0,12,8*1C\xd\xa"); // Full Cold Restart (old receivers can ignore that)
	std::vector<sirf_legacy::tContentPSRF_TXT> Contents = hidden::ReceiveResponse<tPacketNMEANoCRC, sirf_legacy::tContentPSRF_TXT>(port, 9);
	if (Contents.empty())
		return {};
	tDataSetHW DataSetHW{};
	DataSetHW.Chip = "SiRF";
	if (port.GetBaudRate() != dsConfig.GetUART().BR)
	{
		port.Send("$PSRF100,1,9600,8,1,0*0D\xd\xa");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		port.SetBaudRate(dsConfig.GetUART().BR);
		if (!hidden::ReceiveAnyNMEA<tPacketNMEA>(port))
			return { { DataSetHW, tStatus::InitSetBaudrate } };
	}
	std::optional<tDataSetHW> DataSetHWOpt = TrySiRF_Parse_Legacy(Contents, DataSetHW);
	if (DataSetHWOpt.has_value())
		return { { *DataSetHWOpt, tStatus::None } };
	DataSetHWOpt = TrySiRF_Parse(Contents, DataSetHW);
	if (DataSetHWOpt.has_value())
		return { { *DataSetHWOpt, tStatus::None } };
	return { { DataSetHW, tStatus::InitNotSupported } };
}

}
}
