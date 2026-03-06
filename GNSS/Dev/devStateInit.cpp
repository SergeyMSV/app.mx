#include "devStateInit.h"

namespace dev
{
namespace state
{

std::optional<std::pair<tDataSetHW, tStatus>> TryMTK(tPortUART& port);
std::optional<std::pair<tDataSetHW, tStatus>> TrySiRF(tPortUART& port, const dev::tDataSetConfig& dsConfig);

std::uint32_t SelectBR(tPortUART& port)
{
	if (hidden::ReceiveAnyNMEA<tPacketNMEA>(port)) // It tests at the default baudrate.
		return port.GetBaudRate();
	// When the connected receiver is identified its baudrate shall be set to 9600 bauds per second.
	const std::uint32_t BRs[] = { 4800, 9600 };
	for (auto i : BRs)
	{
		port.SetBaudRate(i);
		if (hidden::ReceiveAnyNMEA<tPacketNMEA>(port))
			return i;
	}
	return 0;
}

std::pair<tDataSetHW, tStatus> Init(tPortUART& port, const tDataSetConfig& dsConfig)
{
	std::uint32_t BaudRate = SelectBR(port);

	std::optional<std::pair<tDataSetHW, tStatus>> DsHWStatusOpt;
	DsHWStatusOpt = TrySiRF(port, dsConfig);
	if (DsHWStatusOpt.has_value())
		return *DsHWStatusOpt;
	DsHWStatusOpt = TryMTK(port);
	if (DsHWStatusOpt.has_value())
		return *DsHWStatusOpt;
	return { {}, tStatus::InitNotSupported };
}

}
}
