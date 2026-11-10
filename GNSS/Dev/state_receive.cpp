#include "dataset_gnss.h"
#include "state.h"
#include "state_policy.h"

#include <shareUtilsFile.h>

#include <optional>

#ifdef GNSS_TEST
#include <iostream>
#endif // GNSS_TEST

namespace dev
{
namespace state
{

template <typename TPolicy>
class tContentSet
{
	int m_MsgCount = 0;
	bool m_Error = false;

	tDataSetGNSS m_DataSetGNSS{};

public:
	bool operator()(const tPacketNMEA& packet, tLog& a_log)
	{
		{
			std::optional<typename TPolicy::tContentGSV> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentGSV>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
#ifdef GNSS_TEST
				std::cout << " " << MsgOpt->GetID();
#endif // GNSS_TEST
				for (const auto& i : MsgOpt->Sats)
				{
					this->m_DataSetGNSS.Satellites.emplace_back(static_cast<std::uint8_t>(i.ID.GetValue()), static_cast<std::uint8_t>(i.Elevation.GetValue()), static_cast<std::uint16_t>(i.Azimuth.GetValue()), static_cast<std::uint8_t>(i.SNR.GetValue()));
				}
				return this->CheckOrder(TPolicy::NumGSV);
			}
		}

		{
			std::optional<typename TPolicy::tContentGGA> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentGGA>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
#ifdef GNSS_TEST
				std::cout << " " << MsgOpt->GetID();
#endif // GNSS_TEST
				this->m_DataSetGNSS.Altitude = MsgOpt->Altitude.GetValue();
				switch (MsgOpt->GNSS.Value)
				{
				case utils::packet::nmea::type::tGNSS_State::GPS: this->m_DataSetGNSS.GpHDOP = MsgOpt->HDOP.GetValue(); break; // for GPS receivers
				case utils::packet::nmea::type::tGNSS_State::GlobalNavigation: this->m_DataSetGNSS.GnHDOP = MsgOpt->HDOP.GetValue(); break;
				}
				return this->CheckOrder(TPolicy::NumGGA);
			}
		}

		{
			std::optional<typename TPolicy::tContentGLL> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentGLL>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
#ifdef GNSS_TEST
				std::cout << " " << MsgOpt->GetID();
#endif // GNSS_TEST
				//...
				return this->CheckOrder(TPolicy::NumGLL);
			}
		}

		{
			std::optional<typename TPolicy::tContentGSA> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentGSA>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
#ifdef GNSS_TEST
				std::cout << " " << MsgOpt->GetID();
#endif // GNSS_TEST
				switch (MsgOpt->GNSS.Value)
				{
				case utils::packet::nmea::type::tGNSS_State::GPS: this->m_DataSetGNSS.GpHDOP = MsgOpt->HDOP.GetValue(); break;
				case utils::packet::nmea::type::tGNSS_State::GLONASS: this->m_DataSetGNSS.GlHDOP = MsgOpt->HDOP.GetValue(); break;
				}
				return this->CheckOrder(TPolicy::NumGSA);
			}
		}

		{
			std::optional<typename TPolicy::tContentRMC> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentRMC>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
#ifdef GNSS_TEST
				std::cout << " " << MsgOpt->GetID();
#endif // GNSS_TEST
				if (TPolicy::ReceiverModel == tReceiverModel::SiRF_LR9548S) // This receiver doesn't support ZDA.
				{
					auto [DtSec, DtMSec] = utils::packet::nmea::type::hide::SplitDouble(MsgOpt->Time.GetValue(), 3); // 3 -> milliseconds
					this->m_DataSetGNSS.DateTime = { MsgOpt->Date.GetValue() + DtSec ,  DtMSec };
				}

				this->m_DataSetGNSS.Valid = MsgOpt->Status.GetValue();
				this->m_DataSetGNSS.Latitude = MsgOpt->Latitude.GetValue();
				this->m_DataSetGNSS.Longitude = MsgOpt->Longitude.GetValue();
				this->m_DataSetGNSS.Speed = MsgOpt->Speed.GetValue();
				this->m_DataSetGNSS.Course = MsgOpt->Course.GetValue();
				if constexpr (requires(typename TPolicy::tContentRMC obj) { obj.Mode; })
				{
					this->m_DataSetGNSS.Mode = MsgOpt->Mode.ToStringEx();
					//this->m_DataSetGNSS.Mode = MsgOpt->Mode.GetValue();
				}
				return this->CheckOrder(TPolicy::NumRMC);
			}
		}

		{
			std::optional<typename TPolicy::tContentVTG> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentVTG>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
#ifdef GNSS_TEST
				std::cout << " " << MsgOpt->GetID();
#endif // GNSS_TEST
				//...
				return this->CheckOrder(TPolicy::NumVTG);
			}
		}

		{
			std::optional<typename TPolicy::tContentZDA> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentZDA>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
				auto [DateTime, mSec] = MsgOpt->GetDateTime();
				this->m_DataSetGNSS.DateTime = { DateTime, mSec };
				return this->CheckOrder(TPolicy::NumZDA);
			}
		}

		auto PacketFoundPayload = packet.GetPayloadValue();
		if (PacketFoundPayload.empty() ||
			PacketFoundPayload[0][2] == 'V' ||
			PacketFoundPayload[0][2] == 'Z' ||
			PacketFoundPayload[0][3] == 'S')
		{
			return false;
		}

		std::string msgFaulty = "ER:";
		for (auto& item : PacketFoundPayload)
		{
			msgFaulty += " " + item;
		}
		a_log.WriteLine(msgFaulty);
		return false;
	}

	std::optional<tDataSetGNSS> GetDataSetGNSS() const
	{
		if (m_Error)
			return {};
		return m_DataSetGNSS;
	}

 private:
	bool CheckOrder(int numMsg)
	{
		if (numMsg == tContentSet_Msg_Skip)
			return false;
		if (numMsg == tContentSet_Msg_Last) // the last one
			return true;
		if (m_MsgCount != numMsg)
		{
			m_Error = true;
			return true;
		}
		++m_MsgCount;
		return false;
	}
};

template <typename TContentSet>
tStatus ReceiveInternal(tPortUART& a_port, const tConfig& a_config, const tDataSetHW& a_dsHW, tLog& a_log)
{
	TContentSet ContentSet;
	std::vector<std::uint8_t> DataRaw;
	while (true)
	{
		const std::vector<std::uint8_t> Data = a_port.GetReceived();
		if (Data.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
		DataRaw.insert(DataRaw.end(), Data.begin(), Data.end());
		while (true)
		{
			std::optional<tPacketNMEA> PacketFoundOpt = tPacketNMEA::Find(DataRaw);
			if (!PacketFoundOpt.has_value())
				break;
			if (ContentSet(*PacketFoundOpt, a_log))
			{
				std::optional<tDataSetGNSS> DataSetGNSSOpt = ContentSet.GetDataSetGNSS();
				if (DataSetGNSSOpt.has_value())
				{
					DataSetGNSSOpt->ReceiverModel = a_dsHW.Model;
#ifdef GNSS_TEST
					std::cout << " -> " << DataSetGNSSOpt->ToString() << '\n';
#else // GNSS_TEST
					share::SaveOutFile(a_config.GetOutFile(), DataSetGNSSOpt->ToJSON());
#endif // GNSS_TEST
				}
				ContentSet = {};
			}
		}
	}
	return tStatus::Unknown; // [TBD] check it - Actially it shouldn't be here.
}

tStatus Receive(tPortUART& a_port, const tConfig& a_dsConfig, const tDataSetHW& a_dsHW, tLog& a_log)
{
	if (a_dsHW.Chip == "MTK")
	{
		switch (a_dsHW.ModelID)
		{
		case tReceiverModel::MTK_SC872_A: return ReceiveInternal<tContentSet<tPolicyMTK_SC872_A>>(a_port, a_dsConfig, a_dsHW, a_log);
		case tReceiverModel::MTK_EB_800A: return ReceiveInternal<tContentSet<tPolicyMTK_EB_800A>>(a_port, a_dsConfig, a_dsHW, a_log);
		default: break;
		}
		return tStatus::ReceiveNotSupportedModel;
	}

	if (a_dsHW.Chip == "SiRF")
	{
		switch (a_dsHW.ModelID)
		{
		case tReceiverModel::SiRF_GSU_7x: return ReceiveInternal<tContentSet<tPolicySiRF_GSU_7x>>(a_port, a_dsConfig, a_dsHW, a_log);
		case tReceiverModel::SiRF_LR9548S: return ReceiveInternal<tContentSet<tPolicySiRF_LR9548S>>(a_port, a_dsConfig, a_dsHW, a_log);
		default: break;
		}
		return tStatus::ReceiveNotSupportedModel;
	}

	// [TBD] generic

	return tStatus::ReceiveNotSupportedChip;
}

std::string ToString(tStatus a_status)
{
	switch (a_status)
	{
	case tStatus::None: return "";
	case tStatus::InitSetBaudrate: return "init.: the baudrate is not appropriate";
	case tStatus::InitNotSupported: return "init.: the equipment is not supported";
	case tStatus::InitNotResponded: return "init.: the equipment does not respond";
	case tStatus::InitFailed: return "init.: the response is not successful";
	case tStatus::ReceiveNotSupportedChip: return "recv.: the chip is not supported";
	case tStatus::ReceiveNotSupportedModel: return "recv.: the equipment model is not supported";
	case tStatus::Unknown:
	default:
		return "an unknown error has occurred";
	}
}

}
}
