#include "devDataSetConfig.h"
#include "main.h"
#include "main_server.h"

#include <algorithm>
#include <memory>

void ThreadPortDEMO(const std::shared_ptr<dev::tDataSetConfig>& config, tTWRServer& server, tTWRQueueSPICmd& queueIn)
{
	std::weak_ptr<dev::tDataSetConfig> ConfigWeak(config);

	while (!ConfigWeak.expired())
	{
		if (queueIn.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		boost::shared_ptr<std::vector<std::uint8_t>> PacketRsp;

		tPacketTWRCmdEp Cmd = queueIn.get_front();
		switch (Cmd.Value.GetMsgId())
		{
		case tTWRMsgId::DEMO_Request:
		{
			std::vector<std::uint8_t> Data = Cmd.Value.GetPayload();
			std::reverse(Data.begin(), Data.end());
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make(Cmd.Value, Data).ToVector()));
			break;
		}
		}

		if (PacketRsp.get() == nullptr)
			PacketRsp.reset(new std::vector<std::uint8_t>(tTWRPacketRsp::Make_ERR(Cmd.Value, tTWRMsgStatus::NotSupported).ToVector()));

		server.Send(Cmd.Endpoint, PacketRsp);
	}
}
