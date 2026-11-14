#pragma once

#include "dataset_gnss.h"
#include "state.h"
#include "state_policy.h"

#include <optional>

#ifdef GNSS_TEST
#include <iostream>
#endif // GNSS_TEST

namespace dev
{
namespace state
{

enum class tContentType
{
	None,
	GNSS,
	Restart,
	Error,
};

template <typename TPolicy>
class tContentSet
{
	int m_MsgCount = 0;
	bool m_Error = false;

	tDataSetGNSS m_DataSetGNSS{};

public:
	tContentType operator()(const tPacketNMEA& packet, tLog& a_log)
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
				return this->IsLastMsg(TPolicy::NumGSV);
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
				case utils::packet::nmea::type::tGNSS_State::None: break;
				case utils::packet::nmea::type::tGNSS_State::GPS: this->m_DataSetGNSS.GpHDOP = MsgOpt->HDOP.GetValue(); break; // for GPS receivers
				case utils::packet::nmea::type::tGNSS_State::GLONASS: break;
				case utils::packet::nmea::type::tGNSS_State::GlobalNavigation: this->m_DataSetGNSS.GnHDOP = MsgOpt->HDOP.GetValue(); break;
				}
				return this->IsLastMsg(TPolicy::NumGGA);
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
				return this->IsLastMsg(TPolicy::NumGLL);
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
				return this->IsLastMsg(TPolicy::NumGSA);
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
				return this->IsLastMsg(TPolicy::NumRMC);
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
				return this->IsLastMsg(TPolicy::NumVTG);
			}
		}

		if constexpr (!std::is_same_v<typename TPolicy::tContentZDA, tContentNotSupported>)
		{
			std::optional<typename TPolicy::tContentZDA> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentZDA>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
				auto [DateTime, mSec] = MsgOpt->GetDateTime();
				this->m_DataSetGNSS.DateTime = { DateTime, mSec };
				return this->IsLastMsg(TPolicy::NumZDA);
			}
		}

		if constexpr (!std::is_same_v<typename TPolicy::tContentP, tContentNotSupported>)
		{
			std::optional<typename TPolicy::tContentP> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentP>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
				a_log.WriteLine(MakeLogMsg("P", packet.GetPayloadValue()));
				return HandleContentP(*MsgOpt);
			}
		}

		a_log.WriteLine(MakeLogMsg("ER", packet.GetPayloadValue()));
		return tContentType::None;
	}

	std::optional<tDataSetGNSS> GetDataSetGNSS() const
	{
		if (m_Error)
			return {};
		return m_DataSetGNSS;
	}

protected:
	virtual tContentType HandleContentP(const typename TPolicy::tContentP& a_content) { return tContentType::None; }

private:
	tContentType IsLastMsg(int numMsg)
	{
		if (numMsg == tContentSet_Msg_Skip)
			return tContentType::None;
		if (numMsg == tContentSet_Msg_Last) // the last one
			return tContentType::GNSS;
		if (m_MsgCount != numMsg)
		{
			m_Error = true;
			return tContentType::Error;
		}
		++m_MsgCount;
		return tContentType::None;
	}

	template<typename T>
	static std::string MakeLogMsg(const std::string& caption, const T& payload)
	{
		std::string msg = caption + ":";
		for (auto& item : payload)
		{
			msg += " " + item;
		}
		return msg;
	}
};

}
}
