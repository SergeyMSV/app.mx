#include "devStateReceive.h"

namespace dev
{
namespace state
{

struct tTypesSiRF_GSU_7x
{
	static constexpr std::string_view Model = "GSU-7x";
	static constexpr int NumGGA = 0;
	static constexpr int NumGLL = 1;
	static constexpr int NumGSA = 2;
	static constexpr int NumGSV = hidden::tContentSet_Msg_Skip;
	static constexpr int NumRMC = 3;
	static constexpr int NumVTG = 4;
	static constexpr int NumZDA = hidden::tContentSet_Msg_Last;
	using tContentGGA = utils::packet::nmea::sirf_gsu_7x::tContentGGA;
	using tContentGLL = utils::packet::nmea::sirf_gsu_7x::tContentGLL;
	using tContentGSA = utils::packet::nmea::sirf_gsu_7x::tContentGSA;
	using tContentGSV = utils::packet::nmea::sirf_gsu_7x::tContentGSV;
	using tContentRMC = utils::packet::nmea::sirf_gsu_7x::tContentRMC;
	using tContentVTG = utils::packet::nmea::sirf_gsu_7x::tContentVTG;
	using tContentZDA = utils::packet::nmea::sirf_gsu_7x::tContentZDA;

	static constexpr double Default_Latitude = 36.0;
	static constexpr double Default_Longitude = 136.0;
	static constexpr double Default_Speed = 9999.99;
	static constexpr double Default_Course = 999.99;
};

struct tTypesSiRF_LR9548S
{
	static constexpr std::string_view Model = "LR9548S";
	static constexpr int NumGGA = 0;
	static constexpr int NumGLL = hidden::tContentSet_Msg_Skip;
	static constexpr int NumGSA = hidden::tContentSet_Msg_Skip;
	static constexpr int NumGSV = hidden::tContentSet_Msg_Skip;
	static constexpr int NumRMC = 1;
	static constexpr int NumVTG = hidden::tContentSet_Msg_Last;
	static constexpr int NumZDA = hidden::tContentSet_Msg_Skip;
	using tContentGGA = utils::packet::nmea::sirf_lr9548s::tContentGGA;
	using tContentGLL = utils::packet::nmea::sirf_lr9548s::tContentGLL;
	using tContentGSA = utils::packet::nmea::sirf_lr9548s::tContentGSA;
	using tContentGSV = utils::packet::nmea::sirf_lr9548s::tContentGSV;
	using tContentRMC = utils::packet::nmea::sirf_lr9548s::tContentRMC;
	using tContentVTG = utils::packet::nmea::sirf_lr9548s::tContentVTG;
	using tContentZDA = utils::packet::nmea::sirf_gsu_7x::tContentZDA; // The receiver doesn't support ZDA, therefore any similar type of message is acceptable just as a placeholder.
};

tStatus ReceiveSiRF(tPortUART& port, const tDataSetHW& dsHW)
{
	if (dsHW.Model == tTypesSiRF_GSU_7x::Model)
		return hidden::Receive<hidden::tContentSet<tTypesSiRF_GSU_7x>>(port);
	if (dsHW.Model == tTypesSiRF_LR9548S::Model)
		return hidden::Receive<hidden::tContentSet<tTypesSiRF_LR9548S>>(port);
	return tStatus::ReceiveNotSupportedModel;
}

}
}
