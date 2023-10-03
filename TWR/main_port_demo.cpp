#include "devDataSetConfig.h"
#include "main.h"
#include "main_server.h"

#include <algorithm>
#include <memory>

void Thread_Port_DEMO(const std::shared_ptr<dev::tDataSetConfig>& dataSetConfig, tTWRServer& twrServer, tTWRQueueDEMOCmd& twrQueue)
{
	std::weak_ptr<dev::tDataSetConfig> DataSetConfig_WeakPtr(dataSetConfig);

	while (true)
	{
		if (DataSetConfig_WeakPtr.expired())
			return;

		if (!twrQueue.empty())
		{
			boost::shared_ptr<tVectorUInt8> PacketRsp;

			tPacketTWRCmdEp Cmd = twrQueue.get_front();
			switch (Cmd.Value.GetMsgId())
			{
			case TWR::tMsgId::DEMO_Request:
			{
				tVectorUInt8 Data = Cmd.Value.GetPayload();
				std::reverse(Data.begin(), Data.end());
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make(Cmd.Value, Data).ToVector()));
				break;
			}
			}

			if (PacketRsp.get() == nullptr)
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd.Value, TWR::tMsgStatus::NotSupported).ToVector()));

			twrServer.Send(Cmd.Endpoint, PacketRsp);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}
