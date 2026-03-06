#pragma once

#include <devConfig.h>

#include "devDataSetGNSS.h"
#include "devDataSetHW.h"
#include "devPortUART.h"
#include "devState.h"

#include <utilsPacketNMEAPayload.h>

#include <optional>

#include <iostream> // [TEST]

namespace dev
{
namespace state
{
namespace hidden
{

constexpr int tContentSet_Msg_Last = 100;
constexpr int tContentSet_Msg_Skip = -1;

template <typename TPolicy>
class tContentSet
{
	int m_MsgCount = 0;
	bool m_Error = false;

	tDataSetGNSS m_DataSetGNSS{};

public:
	tContentSet()
	{
		m_DataSetGNSS.ReceiverModel = TPolicy::Model;
	}

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
				//std::cout << " " << MsgOpt->GetID();
				auto [DtSec, DtMSec] = utils::packet::nmea::type::hidden::SplitDouble(MsgOpt->Time.GetValue(), 3); // 3 -> milliseconds
				this->m_DataSetGNSS.DateTime = { MsgOpt->Date.GetValue() + DtSec ,  DtMSec };
				this->m_DataSetGNSS.Valid = MsgOpt->Status.GetValue();
				this->m_DataSetGNSS.Latitude = MsgOpt->Latitude.GetValue();
				this->m_DataSetGNSS.Longitude = MsgOpt->Longitude.GetValue();
				this->m_DataSetGNSS.Speed = MsgOpt->Speed.GetValue();
				this->m_DataSetGNSS.Course = MsgOpt->Course.GetValue();
				this->m_DataSetGNSS.Mode = MsgOpt->Mode.GetValue();
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
				std::cout << " " << MsgOpt->GetID();
				//...
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
		if (numMsg == hidden::tContentSet_Msg_Skip)
			return false;
		if (numMsg == hidden::tContentSet_Msg_Last) // the last one
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
tStatus Receive(tPortUART& port)
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
					//.ReceiverModel = dsHW.Model
					// [TBD] save data into a file
					// 
					// global_dataSet = ContentSet.GetDataSetGNSS();
					std::cout << " -> " << DataSetGNSSOpt->ToString() << '\n';
				}
				ContentSet = {};
			}
		}
	}
	return tStatus::Unknown; // [TBD] check it - Actially it shouldn't be here.
}

}

tStatus Receive(tPortUART& port, const tDataSetHW& dsHW);

}
}
