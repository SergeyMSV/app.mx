#include "main_server.h"

tVectorUInt8 tTWRServer::HandlePacket(tPacketTWRCmd& cmd)
{
	switch (cmd.GetMsgId())
	{
	case TWR::tMsgId::GetVersion: return tPacketTWRRsp::Make(cmd, Version).ToVector();
	case TWR::tMsgId::DEMO_Request:
	case TWR::tMsgId::SPI_Request:
	case TWR::tMsgId::SPI_GetSettings:
	case TWR::tMsgId::SPI_SetChipControl:
	{
		if (PutInQueue(cmd))
			return {};
	}
	}
	return tPacketTWRRsp::Make_ERR(cmd, TWR::tMsgStatus::NotSupported).ToVector();
}

bool tTWRServer::PutInQueue(const tPacketTWRCmd& cmd)
{
	switch (cmd.GetEndpoint())
	{
	case TWR::tEndpoint::DEMO:
	{
		TWRQueue.DEMO.push_back(cmd);
		return true;
	}
	case TWR::tEndpoint::SPI0_CS0:
	{
		TWRQueue.SPI_0_0.push_back(cmd);
		return true;
	}
	//case TWR::tEndpoint::SPI0_CS1:
	//case TWR::tEndpoint::SPI0_CS2:
	}
	return false;
}
