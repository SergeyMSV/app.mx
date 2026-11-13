#include "state_init.h"
#include "state_policy.h"

#include <utilsPacketNMEAPayload.h>
#include <utilsPacketNMEAPayloadPMTK.h>
#include <utilsPacketNMEAPayloadPTWS.h>

namespace dev
{
namespace state
{

std::uint32_t SelectBR(tPortUART& port);

std::optional<std::pair<tDataSetHW, tStatus>> TryMTK(tPortUART& a_port, const tConfig& a_config)
{
	using namespace utils::packet::nmea;
	using namespace utils::packet::nmea::mtk;

	{
		a_port.Send("$PMTK000*32\xd\xa"); // echo - that means that chip is MTK
		std::vector<tContentPMTK_TEST> rsp = ReceiveResponse<tPacketNMEA, tContentPMTK_TEST>(a_port);
		if (rsp.empty())
			return {};
	}

	tDataSetHW dsHW{};
	dsHW.Chip = "MTK";
	{
		a_port.Send("$PMTK605*31\xd\xa");
		std::vector<tContentPMTK> rsp = ReceiveResponse<tPacketNMEA, tContentPMTK>(a_port, 1);
		if (rsp.empty())
			return { { dsHW, tStatus::InitNotResponded } };
		if (rsp[0].Value.size() == 6 && rsp[0].Value[1] == "705")
		{
			dsHW.Firmware = rsp[0].Value[2];
			dsHW.FirmwareID = rsp[0].Value[3];
			dsHW.Model = rsp[0].Value[4];
		}
	}

	if (!dsHW.Model.empty())
	{
		if (dsHW.Model == "EB-800A")
			dsHW.ModelID = tReceiverModel::MTK_EB_800A;
	}
	else
	{
		a_port.Send("$PTWS,VERSION,GET*0C\xd\xa");
		std::vector<telit::tContentVERSION> rsp = ReceiveResponse<tPacketNMEA, telit::tContentVERSION> (a_port, 1);
		if (!rsp.empty())
		{
			dsHW.Manufacturer = "Telit";
			dsHW.Model = rsp[0].Version;
			if (dsHW.Model == "MT33-v3.8.4-STD-2.1.008-N96") // "SC872-A"
				dsHW.ModelID = tReceiverModel::MTK_SC872_A;
		}
	}

	for (int i = 0; i < 2; ++i) // The baudrate can be changed once.
	{
		//a_port.Send("$PMTK103*30\xd\xa"); // Cold Restart
		a_port.Send("$PMTK104*37\xd\xa"); // Full Cold Restart	-	The Serial Port setting will be reset to the default value.
		// $PMTK011,MTKGPS*08
		// $PMTK010,001*2E
		// $PMTK011,MTKGPS*08
		// $PMTK010,002*2D
		std::vector<tContentPMTK> rsp = ReceiveResponse<tPacketNMEA, tContentPMTK>(a_port, 4);
		if (rsp.empty())
		{
			if (i == 1)
				return { { dsHW, tStatus::InitNotResponded } };
			SelectBR(a_port);
		}
	}

	if (!SetSerialPortBR<tPolicyMTK>(a_port, a_config))
		return { { dsHW, tStatus::InitSetBaudrate } };

	{
		a_port.Send("$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0*29\xd\xa"); // GLL,RMC,VTG,GGA,GSA,GSV,...,ZDA once a second
		std::vector<tContentACK> rsp = ReceiveResponse<tPacketNMEA, tContentACK>(a_port, 1);
		if (rsp.empty())
			return { { dsHW, tStatus::InitNotResponded } };
		if (rsp[0].Status.GetValue() != static_cast<std::uint8_t>(tContentACK::tStatus::Succeeded))
			return { { dsHW, tStatus::InitFailed } };
	}
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	return { { dsHW, tStatus::None } };
}

}
}
