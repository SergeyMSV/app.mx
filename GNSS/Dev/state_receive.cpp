#include "state_receive_mtk.h"
#include "state_receive_sirf.h"

#include <shareUtilsFile.h>

namespace dev
{
namespace state
{

template <typename TContentSet>
tStatus ReceiveInternal(tPortUART& a_port, const tConfig& a_config, const tDataSetHW& a_dsHW, tLog& a_log)
{
	auto timeReceived = std::chrono::steady_clock::now();
	TContentSet ContentSet;
	std::vector<std::uint8_t> DataRaw;
	while (true)
	{
		const std::vector<std::uint8_t> Data = a_port.GetReceived();
		if (Data.empty())
		{
			auto period = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - timeReceived);
			if (period.count() > 2) // [#] No data was received in the last 2 seconds.
				return tStatus::NoCompleteSequenceNMEA;

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}
		DataRaw.insert(DataRaw.end(), Data.begin(), Data.end());
		while (true)
		{
			std::optional<tPacketNMEA> PacketFoundOpt = tPacketNMEA::Find(DataRaw);
			if (!PacketFoundOpt.has_value())
				break;
			switch (ContentSet(*PacketFoundOpt, a_log))
			{
			case tContentType::None: break;
			case tContentType::GNSS:
			case tContentType::Error:
			{
				std::optional<tDataSetGNSS> DataSetGNSSOpt = ContentSet.GetDataSetGNSS();
				if (DataSetGNSSOpt.has_value())
				{
					timeReceived = std::chrono::steady_clock::now();

					DataSetGNSSOpt->ReceiverModel = a_dsHW.Model;
#ifdef GNSS_TEST
					std::cout << " -> " << DataSetGNSSOpt->ToString() << '\n';
#else // GNSS_TEST
					share::SaveOutFile(a_config.GetOutFile(), DataSetGNSSOpt->ToJSON());
#endif // GNSS_TEST
				}
				ContentSet = {};
				break;
			}
			case tContentType::Restart:
				return tStatus::RestartDetected;
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
		case tReceiverModel::MTK_SC872_A: return ReceiveInternal<tContentSetMTK<tPolicyMTK_SC872_A>>(a_port, a_dsConfig, a_dsHW, a_log);
		case tReceiverModel::MTK_EB_800A: return ReceiveInternal<tContentSetMTK<tPolicyMTK_EB_800A>>(a_port, a_dsConfig, a_dsHW, a_log);
		default: break;
		}
		return tStatus::ReceiveNotSupportedModel;
	}

	if (a_dsHW.Chip == "SiRF")
	{
		switch (a_dsHW.ModelID)
		{
		case tReceiverModel::SiRF_GSU_7x: return ReceiveInternal<tContentSetSiRF<tPolicySiRF_GSU_7x>>(a_port, a_dsConfig, a_dsHW, a_log);
		case tReceiverModel::SiRF_LR9548S: return ReceiveInternal<tContentSetSiRF<tPolicySiRF_LR9548S>>(a_port, a_dsConfig, a_dsHW, a_log);
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
	case tStatus::NoCompleteSequenceNMEA: return "recv.: no complete sequence of NMEA messages was received in the last some seconds";
	case tStatus::RestartDetected: return "recv.: a restart has been detected";
	case tStatus::Unknown:
	default:
		return "an unknown error has occurred";
	}
}

}
}
