#include "state_init.h"
#include "state_policy.h"

#include <utilsPacketNMEAPayload.h>
#include <utilsPacketNMEAPayloadPSRF.h>
#include <utilsString.h>

namespace dev
{
namespace state
{

using namespace utils::packet::nmea;

std::optional<tDataSetHW> TrySiRF_Parse_Legacy(const std::vector<sirf_legacy::tContentPSRF_TXT>& a_rsp, const tDataSetHW& a_dsHW)
{
	if (a_rsp.empty())
		return {};
	tDataSetHW dsHW = a_dsHW;
	for (auto& i : a_rsp)
	{
		if (i.Value.size() != 3)
			continue;
		if (utils::string::Contains(i.Value[2], "Version ")) // $PSRFTXTVersion ...*XX
		{
			dsHW.Manufacturer = "LEADTEK";
			std::vector<std::string_view> strs = utils::string::SplitTrimString(a_rsp[0].Value[2], ' ');
			if (strs.size() == 2)
				dsHW.Firmware = strs[1];
			dsHW.Model = "LR9548S";
			dsHW.ModelID = tReceiverModel::SiRF_LR9548S;
		}
	}
	if (dsHW.Model.empty())
		return {};
	return dsHW;
}

std::optional<tDataSetHW> TrySiRF_Parse(const std::vector<sirf_legacy::tContentPSRF_TXT>& a_rsp, const tDataSetHW& a_dsHW)
{
	if (a_rsp.empty())
		return {};
	tDataSetHW dsHW = a_dsHW;
	dsHW.Chip = "SiRF";
	for (auto& i : a_rsp)
	{
		if (i.Value.size() < 3)
			continue;
		std::vector<std::string_view> strs = utils::string::SplitTrimString(i.Value[2], ':');
		if (strs.size() < 2)
			continue;
		if (utils::string::Contains(strs[0], "GSU"))
		{
			dsHW.Model = strs[0];
			dsHW.ModelID = tReceiverModel::SiRF_GSU_7x;
			dsHW.Manufacturer = strs[1];
			if (i.Value.size() == 4)
				dsHW.Manufacturer += "," + i.Value[3];
		}
		else if (strs[0] == "Firmware Checksum")
		{
			dsHW.FirmwareID = strs[1];
		}
	}
	if (dsHW.Model.empty())
		return {};
	return dsHW;
}

std::optional<std::pair<tDataSetHW, tStatus>> TrySiRF(tPortUART& a_port, const tConfig& a_config)
{
	a_port.Send("$PSRF101,0,0,0,0,0,0,12,1*15\xd\xa"); // Hot Restart
	//a_port.Send("$PSRF101,0,0,0,0,0,0,12,2*16\xd\xa"); // Warm Restart
	//a_port.Send("$PSRF101,0,0,0,0,0,0,12,4*10\xd\xa"); // Cold Restart
	//a_port.Send("$PSRF101,0,0,0,0,0,0,12,8*1C\xd\xa"); // Full Cold Restart (old receivers can ignore that)
	std::vector<sirf_legacy::tContentPSRF_TXT> rsp = ReceiveResponse<tPacketNMEANoCRC, sirf_legacy::tContentPSRF_TXT>(a_port, 9);
	if (rsp.empty())
		return {};
	tDataSetHW dsHW{};
	dsHW.Chip = "SiRF";
	std::optional<tDataSetHW> dsHWOpt = TrySiRF_Parse_Legacy(rsp, dsHW);
	if (!dsHWOpt.has_value())
		dsHWOpt = TrySiRF_Parse(rsp, dsHW);
	if (!dsHWOpt.has_value())
		return { { dsHW, tStatus::InitNotSupported } };

	if (!SetSerialPortBR<tPolicySiRF>(a_port, a_config))
		return { { dsHW, tStatus::InitSetBaudrate } };

	if (dsHWOpt->ModelID == tReceiverModel::SiRF_LR9548S)
	{
		a_port.Send("$PSRF103,02,00,01,01*27\xd\xa"); // GSA is send each second with checksum enabled.
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		a_port.Send("$PSRF103,03,00,01,01*26\xd\xa"); // GSV is send each second with checksum enabled.
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//a_port.Send("$PSRF103,08,00,01,01*2D\xd\xa"); // ZDA is send each second with checksum enabled.
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return { { *dsHWOpt, tStatus::None } };
}

}
}
