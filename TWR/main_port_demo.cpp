#include "devDataSetConfig.h"
#include "main.h"
#include "main_server.h"

#include <algorithm>

void Thread_Port_DEMO(tTWRServer& twrServer, tTWRQueueDEMOCmd& twrQueue)
{
	while (true)
	{
		if (!twrQueue.empty())
		{
			boost::shared_ptr<tVectorUInt8> PacketRsp;

			tPacketTWRCmd Cmd = twrQueue.front();
			switch (Cmd.GetMsgId())
			{
			case TWR::tMsgId::DEMO_Request:
			{
				tVectorUInt8 Data = Cmd.GetPayload();
				std::reverse(Data.begin(), Data.end());
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make(Cmd, Data).ToVector()));
				break;
			}
			}

			if (PacketRsp.get() == nullptr)
				PacketRsp.reset(new tVectorUInt8(tPacketTWRRsp::Make_ERR(Cmd, TWR::tMsgStatus::NotSupported).ToVector()));

			twrServer.Send(PacketRsp);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}
