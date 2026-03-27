#include "devStateInit.h"

#include "devStatePolicy.h"

#include <utilsPacketNMEAPayload.h>
#include <utilsPacketNMEAPayloadPMTK.h>
#include <utilsPacketNMEAPayloadPTWS.h>

namespace dev
{
namespace state
{

std::uint32_t SelectBR(tPortUART& port);

std::optional<std::pair<tDataSetHW, tStatus>> TryMTK(tPortUART& port, const tDataSetConfig& dsConfig)
{
	using namespace utils::packet::nmea;
	using namespace utils::packet::nmea::mtk;

	port.Send("$PMTK000*32\xd\xa"); // echo - that means that chip is MTK
	std::vector<tContentPMTK> Contents = ReceiveResponse<tPacketNMEA, tContentPMTK>(port, 1);
	if (Contents.empty())
		return {};
	tDataSetHW DataSetHW{};
	DataSetHW.Chip = "MTK";
	port.Send("$PMTK605*31\xd\xa");
	Contents = ReceiveResponse<tPacketNMEA, tContentPMTK>(port, 1);
	if (Contents.empty())
		return { { DataSetHW, tStatus::InitNotResponded } };
	if (Contents[0].Value.size() == 6 && Contents[0].Value[1] == "705")
	{
		DataSetHW.Firmware = Contents[0].Value[2];
		DataSetHW.FirmwareID = Contents[0].Value[3];
		DataSetHW.Model = Contents[0].Value[4];
	}

	if (!DataSetHW.Model.empty())
	{
		if (DataSetHW.Model == "EB-800A")
			DataSetHW.ModelID = tReceiverModel::MTK_EB_800A;
	}
	else
	{
		port.Send("$PTWS,VERSION,GET*0C\xd\xa");
		std::vector<telit::tContentVERSION> Contents = ReceiveResponse<tPacketNMEA, telit::tContentVERSION> (port, 1);
		if (!Contents.empty())
		{
			DataSetHW.Manufacturer = "Telit";
			DataSetHW.Model = Contents[0].Version;
			if (DataSetHW.Model == "MT33-v3.8.4-STD-2.1.008-N96") // "SC872-A"
				DataSetHW.ModelID = tReceiverModel::MTK_SC872_A;
		}
	}

	for (int i = 0; i < 2; ++i) // The baudrate can be changed once.
	{
		//port.Send("$PMTK103*30\xd\xa"); // Cold Restart
		port.Send("$PMTK104*37\xd\xa"); // Full Cold Restart	-	The Serial Port setting will be reset to the default value.
		// $PMTK011,MTKGPS*08
		// $PMTK010,001*2E
		// $PMTK011,MTKGPS*08
		// $PMTK010,002*2D
		Contents = ReceiveResponse<tPacketNMEA, tContentPMTK>(port, 4);
		if (Contents.empty())
		{
			if (i == 1)
				return { { DataSetHW, tStatus::InitNotResponded } };
			SelectBR(port);
		}
	}

	if (!SetSerialPortBR<tPolicyMTK>(port, dsConfig))
		return { { DataSetHW, tStatus::InitSetBaudrate } };

	port.Send("$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0*29\xd\xa"); // GLL,RMC,VTG,GGA,GSA,GSV,...,ZDA once a second
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	return { { DataSetHW, tStatus::None } };
}

}
}
