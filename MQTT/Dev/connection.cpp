#include "connection.h"
#include "hwmon.h"

#include <shareMQTT.h>

#include <sstream>

#include <ctime>

namespace dev
{

std::string MakeClientID(std::string fid, const std::string& uid)
{
	constexpr int kClientIDSize = 23; // [#] see MQTT protocol
	if (uid.size() > kClientIDSize + 1)
		return {};
	fid.resize(kClientIDSize - (uid.size() + 1), '0');
	return fid + '-' + uid;
}

std::string ToHexString(std::uint8_t value)
{
	std::stringstream SStr;
	SStr << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(value);
	return SStr.str();
}

void TaskConnectionHandler(std::string_view host, std::string_view service, std::shared_ptr<tConfig> сonfig)
{
	constexpr std::uint16_t kKeepAlive = 15; // sec. // [#] - TaskTransactionWait(..) should be taken into consideration
	share::tConnection Connection(host, service, kKeepAlive);

	const std::string ClientID = MakeClientID(сonfig->GetFamily().ID, сonfig->GetUID().ID);

	Connection.Connect(mqtt::tSessionStateRequest::Continue, ClientID, mqtt::tQoS::AtMostOnceDelivery, true, ClientID + "_Will", "Not connected"); // 1883

	Connection.Subscribe({ ClientID + "_Settings", mqtt::tQoS::AtLeastOnceDelivery });

	// Message format: [timespamp],[temperature*1000(°C)],[Humidity*1000(%)]
	const std::vector<dev::tHwmon> Hwmon = dev::GetHwmon();
	const std::time_t TimeNow = std::time(nullptr);
	const std::string PackTime = std::to_string(TimeNow) + ',';
	for (auto& i : Hwmon)
	{
		std::string Packet = PackTime + i.Temperature + ',' + i.Humidity;
		std::string Topic = ClientID + "/hwmon/" + i.Name;
		if (i.Reg)
			Topic += "_" + ToHexString(i.Reg);
		Connection.Publish_AtMostOnceDelivery(true, Topic, Packet);
	}

	Connection.Disconnect();
}

}
