#include "devStateInit.h"

#include <utilsPacketNMEAPayload.h>
#include <utilsPacketNMEAPayloadPMTK.h>
#include <utilsPacketNMEAPayloadPTWS.h>

namespace dev
{
namespace state
{

std::optional<std::pair<tDataSetHW, tStatus>> TryMTK(tPortUART& port)
{
	using namespace utils::packet::nmea;
	using namespace utils::packet::nmea::mtk;

	port.Send("$PMTK000*32\xd\xa"); // echo - that means that chip is MTK
	std::vector<tContentPMTK> Contents = hidden::ReceiveResponse<tPacketNMEA, tContentPMTK>(port, 1);
	if (Contents.empty())
		return {};
	tDataSetHW DataSetHW{};
	DataSetHW.Chip = "MTK";
	port.Send("$PMTK605*31\xd\xa");
	Contents = hidden::ReceiveResponse<tPacketNMEA, tContentPMTK>(port, 1);
	if (Contents.empty())
		return { { DataSetHW, tStatus::InitNotResponded } };
	if (Contents[0].Value.size() == 6 && Contents[0].Value[1] == "705")
	{
		DataSetHW.Firmware = Contents[0].Value[2];
		DataSetHW.FirmwareID = Contents[0].Value[3];
		DataSetHW.Model = Contents[0].Value[4];
	}
	if (DataSetHW.Model.empty())
	{
		port.Send("$PTWS,VERSION,GET*0C\xd\xa");
		std::vector<telit::tContentVERSION> Contents = hidden::ReceiveResponse<tPacketNMEA, telit::tContentVERSION> (port, 1);
		if (!Contents.empty())
		{
			DataSetHW.Manufacturer = "Telit";
			DataSetHW.Model = Contents[0].Version;
		}
	}
	//port.Send("$PMTK103*30\xd\xa"); // Cold Restart
	port.Send("$PMTK104*37\xd\xa"); // Full Cold Restart
	// $PMTK011,MTKGPS*08
	// $PMTK010,001*2E
	// $PMTK011,MTKGPS*08
	// $PMTK010,002*2D
	Contents = hidden::ReceiveResponse<tPacketNMEA, tContentPMTK>(port, 4);
	if (Contents.empty())
		return { { DataSetHW, tStatus::InitNotResponded } };
	return { { DataSetHW, tStatus::None } };
}

}
}
