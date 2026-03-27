#pragma once

#include <utilsPacketNMEA.h>
#include <utilsPacketNMEAPayload.h>
#include <utilsPacketNMEAPayloadPMTK.h>
#include <utilsPacketNMEAPayloadPSRF.h>

namespace dev
{
namespace state
{

namespace placeholder
{
using tContentZDA = utils::packet::nmea::sirf_gsu_7x::tContentZDA;
}

constexpr int tContentSet_Msg_Last = 100;
constexpr int tContentSet_Msg_Skip = -1;

struct tPolicyMTK
{
	static std::vector<std::uint8_t> MakePacketNMEA_SetSerialPort(std::uint32_t baudrate)
	{
		return utils::packet::nmea::tPacketNMEA_Common_CRC(utils::packet::nmea::mtk::tContentPMTK_SetSerialPort(baudrate).GetPayload()).ToVector();
	}
};

struct tPolicyMTK_EB_800A : public tPolicyMTK
{
	static constexpr tReceiverModel ReceiverModel = tReceiverModel::MTK_EB_800A;
	static constexpr int NumGGA = 0;
	static constexpr int NumGLL = 1;
	static constexpr int NumGSA = tContentSet_Msg_Skip; // GPGSA, GLGSA
	static constexpr int NumGSV = tContentSet_Msg_Skip;
	static constexpr int NumRMC = 2;
	static constexpr int NumVTG = 3;
	static constexpr int NumZDA = tContentSet_Msg_Last;
	using tContentGGA = utils::packet::nmea::mtk_eb800a::tContentGGA;
	using tContentGLL = utils::packet::nmea::mtk_eb800a::tContentGLL;
	using tContentGSA = utils::packet::nmea::mtk_eb800a::tContentGSA;
	using tContentGSV = utils::packet::nmea::mtk_eb800a::tContentGSV;
	using tContentRMC = utils::packet::nmea::mtk_eb800a::tContentRMC;
	using tContentVTG = utils::packet::nmea::mtk_eb800a::tContentVTG;
	using tContentZDA = utils::packet::nmea::mtk_eb800a::tContentZDA;
};

struct tPolicyMTK_SC872_A : public tPolicyMTK
{
	static constexpr tReceiverModel ReceiverModel = tReceiverModel::MTK_SC872_A;
	static constexpr int NumGGA = 0;
	static constexpr int NumGLL = tContentSet_Msg_Skip;
	static constexpr int NumGSA = tContentSet_Msg_Skip; // GPGSA, GLGSA
	static constexpr int NumGSV = tContentSet_Msg_Skip;
	static constexpr int NumRMC = 1;
	static constexpr int NumVTG = 2;
	static constexpr int NumZDA = tContentSet_Msg_Last;
	using tContentGGA = utils::packet::nmea::mtk_sc872_a::tContentGGA;
	using tContentGLL = utils::packet::nmea::mtk_sc872_a::tContentGLL;
	using tContentGSA = utils::packet::nmea::mtk_sc872_a::tContentGSA;
	using tContentGSV = utils::packet::nmea::mtk_sc872_a::tContentGSV;
	using tContentRMC = utils::packet::nmea::mtk_sc872_a::tContentRMC;
	using tContentVTG = utils::packet::nmea::mtk_sc872_a::tContentVTG;
	using tContentZDA = utils::packet::nmea::mtk_sc872_a::tContentZDA;
};

struct tPolicySiRF
{
	static std::vector<std::uint8_t> MakePacketNMEA_SetSerialPort(std::uint32_t baudrate)
	{
		return utils::packet::nmea::tPacketNMEA_Common_CRC(utils::packet::nmea::sirf::tContentPSRF_SetSerialPort(baudrate).GetPayload()).ToVector();
	}
};

struct tPolicySiRF_GSU_7x : public tPolicySiRF
{
	static constexpr tReceiverModel ReceiverModel = tReceiverModel::SiRF_GSU_7x;
	static constexpr int NumGGA = 0;
	static constexpr int NumGLL = 1;
	static constexpr int NumGSA = 2;
	static constexpr int NumGSV = tContentSet_Msg_Skip;
	static constexpr int NumRMC = 3;
	static constexpr int NumVTG = 4;
	static constexpr int NumZDA = tContentSet_Msg_Last;
	using tContentGGA = utils::packet::nmea::sirf_gsu_7x::tContentGGA;
	using tContentGLL = utils::packet::nmea::sirf_gsu_7x::tContentGLL;
	using tContentGSA = utils::packet::nmea::sirf_gsu_7x::tContentGSA;
	using tContentGSV = utils::packet::nmea::sirf_gsu_7x::tContentGSV;
	using tContentRMC = utils::packet::nmea::sirf_gsu_7x::tContentRMC;
	using tContentVTG = utils::packet::nmea::sirf_gsu_7x::tContentVTG;
	using tContentZDA = utils::packet::nmea::sirf_gsu_7x::tContentZDA;

	//static constexpr double Default_Latitude = 36.0;
	//static constexpr double Default_Longitude = 136.0;
	//static constexpr double Default_Speed = 9999.99;
	//static constexpr double Default_Course = 999.99;
};

struct tPolicySiRF_LR9548S : public tPolicySiRF
{
	static constexpr tReceiverModel ReceiverModel = tReceiverModel::SiRF_LR9548S;
	static constexpr int NumGGA = 0;
	static constexpr int NumGLL = tContentSet_Msg_Skip;
	static constexpr int NumGSA = 1;
	static constexpr int NumGSV = tContentSet_Msg_Skip;
	static constexpr int NumRMC = 2;
	static constexpr int NumVTG = tContentSet_Msg_Last;
	static constexpr int NumZDA = tContentSet_Msg_Skip;
	using tContentGGA = utils::packet::nmea::sirf_lr9548s::tContentGGA;
	using tContentGLL = utils::packet::nmea::sirf_lr9548s::tContentGLL;
	using tContentGSA = utils::packet::nmea::sirf_lr9548s::tContentGSA;
	using tContentGSV = utils::packet::nmea::sirf_lr9548s::tContentGSV;
	using tContentRMC = utils::packet::nmea::sirf_lr9548s::tContentRMC;
	using tContentVTG = utils::packet::nmea::sirf_lr9548s::tContentVTG;
	using tContentZDA = placeholder::tContentZDA; // The receiver doesn't support ZDA, therefore any similar type of message is acceptable just as a placeholder.
};

}
}
