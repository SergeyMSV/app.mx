#include "devDataSetGNSS.h"
#include "devState.h"
#include "devStatePolicy.h"

#include <optional>

#include <iostream> // [TEST]

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
	bool operator()(const tPacketNMEA& packet)
	{
		{
			std::optional<typename TPolicy::tContentGSV> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentGSV>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
				//std::cout << " " << MsgOpt->GetID();
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
				//std::cout << " " << MsgOpt->GetID();
				this->m_DataSetGNSS.Altitude = MsgOpt->Altitude.GetValue();
				return this->CheckOrder(TPolicy::NumGGA);
			}
		}

		{
			std::optional<typename TPolicy::tContentGLL> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentGLL>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
				std::cout << " " << MsgOpt->GetID();
				//...
				return this->CheckOrder(TPolicy::NumGLL);
			}
		}

		{
			std::optional<typename TPolicy::tContentGSA> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentGSA>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
				std::cout << " " << MsgOpt->GetID();
				//...
				return this->CheckOrder(TPolicy::NumGSA);
			}
		}

		{
			std::optional<typename TPolicy::tContentRMC> MsgOpt = utils::packet::nmea::Parse<typename TPolicy::tContentRMC>(packet.GetPayloadValue());
			if (MsgOpt.has_value())
			{
				if (TPolicy::ReceiverModel == tReceiverModel::SiRF_LR9548S) // This receiver doesn't support ZDA.
				{
					auto [DtSec, DtMSec] = utils::packet::nmea::type::hidden::SplitDouble(MsgOpt->Time.GetValue(), 3); // 3 -> milliseconds
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
				std::cout << " " << MsgOpt->GetID();
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

		for (auto& item : PacketFoundPayload)
			//for (auto& item : packet.GetPayloadValue())
		{
			//utils::packet::nmea::generic::tContentGSV GSV(item);
			std::cout << " " << item;
		}
		std::cout << '\n';
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
tStatus ReceiveInternal(tPortUART& port, const tDataSetConfig& dsConfig, const tDataSetHW& dsHW)
{
	TContentSet ContentSet;
	std::vector<std::uint8_t> DataRaw;
	while (true)
	{
		const std::vector<std::uint8_t> Data = port.GetReceived();
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
			if (ContentSet(*PacketFoundOpt))
			{
				std::optional<tDataSetGNSS> DataSetGNSSOpt = ContentSet.GetDataSetGNSS();
				if (DataSetGNSSOpt.has_value())
				{
					DataSetGNSSOpt->ReceiverModel = dsHW.Model;

					//DataSetGNSSOpt->ToFile(dsConfig.GetOutGNSS()); // [TBD] - block it for windows
					std::cout << " -> " << DataSetGNSSOpt->ToString() << '\n';
				}
				ContentSet = {};
			}
		}
	}
	return tStatus::Unknown; // [TBD] check it - Actially it shouldn't be here.
}

tStatus Receive(tPortUART& port, const tDataSetConfig& dsConfig, const tDataSetHW& dsHW)
{
	if (dsHW.Chip == "MTK")
	{
		switch (dsHW.ModelID)
		{
		case tReceiverModel::MTK_SC872_A: return ReceiveInternal<tContentSet<tPolicyMTK_SC872_A>>(port, dsConfig, dsHW);
		case tReceiverModel::MTK_EB_800A: return ReceiveInternal<tContentSet<tPolicyMTK_EB_800A>>(port, dsConfig, dsHW);
		default: break;
		}
		return tStatus::ReceiveNotSupportedModel;
	}

	if (dsHW.Chip == "SiRF")
	{
		switch (dsHW.ModelID)
		{
		case tReceiverModel::SiRF_GSU_7x: return ReceiveInternal<tContentSet<tPolicySiRF_GSU_7x>>(port, dsConfig, dsHW);
		case tReceiverModel::SiRF_LR9548S: return ReceiveInternal<tContentSet<tPolicySiRF_LR9548S>>(port, dsConfig, dsHW);
		default: break;
		}
		return tStatus::ReceiveNotSupportedModel;
	}

	// [TBD] generic

	return tStatus::ReceiveNotSupportedChip;
}

}
}
