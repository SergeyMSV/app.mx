#include "devStateReceive.h"

namespace dev
{
namespace state
{

struct tTypesMTK_SC872_A
{
	static constexpr std::string_view Model = "MT33-v3.8.4-STD-2.1.008-N96";// "SC872-A";
	static constexpr int NumGGA = 0;
	static constexpr int NumGLL = hidden::tContentSet_Msg_Skip;
	static constexpr int NumGSA = hidden::tContentSet_Msg_Skip;
	static constexpr int NumGSV = hidden::tContentSet_Msg_Skip;
	static constexpr int NumRMC = 1;
	static constexpr int NumVTG = hidden::tContentSet_Msg_Last;
	static constexpr int NumZDA = hidden::tContentSet_Msg_Skip;
	using tContentGGA = utils::packet::nmea::mtk_sc872_a::tContentGGA;
	using tContentGLL = utils::packet::nmea::sirf_gsu_7x::tContentGLL; // The receiver doesn't support GLL, therefore any similar type of message is acceptable just as a placeholder.
	//using tContentGLL = utils::packet::nmea::mtk_sc872_a::tContentGLL;
	using tContentGSA = utils::packet::nmea::mtk_sc872_a::tContentGSA;
	using tContentGSV = utils::packet::nmea::mtk_sc872_a::tContentGSV;
	using tContentRMC = utils::packet::nmea::mtk_sc872_a::tContentRMC;
	using tContentVTG = utils::packet::nmea::mtk_sc872_a::tContentVTG;
	using tContentZDA = utils::packet::nmea::sirf_gsu_7x::tContentZDA; // The receiver doesn't support ZDA, therefore any similar type of message is acceptable just as a placeholder.
};

tStatus ReceiveMTK(tPortUART& port, const tDataSetHW& dsHW)
{
	if (dsHW.Model == tTypesMTK_SC872_A::Model)
		return hidden::Receive<hidden::tContentSet<tTypesMTK_SC872_A>>(port);
	return tStatus::ReceiveNotSupportedModel;
}

}
}
