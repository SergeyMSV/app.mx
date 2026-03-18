#pragma once
#include "devState.h"

#include <utilsPacketNMEA.h>
#include <utilsPacketNMEAPayload.h>

namespace dev
{
namespace state
{

template <typename TPacket, typename TContent>
static std::vector<TContent> ReceiveResponse(tPortUART& port, int maxQty)
{
	std::vector<TContent> Contents;
	std::vector<std::uint8_t> DataRaw;
	for (int i = 0; i < 100 && maxQty > 0; ++i) // Approximately 1 s.
	{
		const std::vector<std::uint8_t> Data = port.GetReceived();
		if (Data.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		DataRaw.insert(DataRaw.end(), Data.begin(), Data.end());
		while (true)
		{
			std::optional<TPacket> PacketFoundOpt = TPacket::Find(DataRaw);
			if (!PacketFoundOpt.has_value())
				break;
			std::optional<TContent> MsgOpt = utils::packet::nmea::Parse<TContent>(PacketFoundOpt->GetPayloadValue());
			if (MsgOpt.has_value())
			{
				Contents.push_back(*MsgOpt);
				--maxQty;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return Contents;
}

template <typename TPacket>
static bool ReceiveAnyNMEA(tPortUART& port)
{
	std::vector<std::uint8_t> DataRaw;
	for (int i = 0; i < 200; ++i) // Approximately 2 s.
	{
		const std::vector<std::uint8_t> Data = port.GetReceived();
		if (Data.empty())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		}
		DataRaw.insert(DataRaw.end(), Data.begin(), Data.end());
		while (true)
		{
			std::optional<TPacket> PacketFoundOpt = TPacket::Find(DataRaw);
			if (!PacketFoundOpt.has_value())
				break;
			return true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return false;
}

template<typename TPolicy>
bool SetSerialPortBR(tPortUART& port, const tDataSetConfig& dsConfig)
{
	if (port.GetBaudRate() == dsConfig.GetUART().BR)
		return true;
	port.Send(TPolicy::MakePacketNMEA_SetSerialPort(dsConfig.GetUART().BR));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	port.SetBaudRate(dsConfig.GetUART().BR);
	return ReceiveAnyNMEA<tPacketNMEA>(port);
}

}
}
