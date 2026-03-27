#include "devStateInit.h"

namespace dev
{
namespace state
{

std::optional<std::pair<tDataSetHW, tStatus>> TryMTK(tPortUART& port, const tDataSetConfig& dsConfig);
std::optional<std::pair<tDataSetHW, tStatus>> TrySiRF(tPortUART& port, const tDataSetConfig& dsConfig);

std::uint32_t SelectBR(tPortUART& port)
{
	const std::uint32_t BR = port.GetBaudRate();
	if (ReceiveAnyNMEA<tPacketNMEA>(port)) // It tests at the default baudrate.
		return BR;
	const std::uint32_t BRs[] = { 4800, 9600, 19200, 38400 };
	for (auto i : BRs)
	{
		port.SetBaudRate(i);
		if (ReceiveAnyNMEA<tPacketNMEA>(port))
			return i;
	}
	port.SetBaudRate(BR); // The baudrate shall be restored if no packets found.
	return 0;
}

std::pair<tDataSetHW, tStatus> Init(tPortUART& port, const tDataSetConfig& dsConfig)
{
	SelectBR(port);

	std::optional<std::pair<tDataSetHW, tStatus>> DsHWStatusOpt;
	DsHWStatusOpt = TrySiRF(port, dsConfig);
	if (!DsHWStatusOpt.has_value())
		DsHWStatusOpt = TryMTK(port, dsConfig);
	if (!DsHWStatusOpt.has_value())
		return { {}, tStatus::InitNotSupported };

	return *DsHWStatusOpt;
}

}
}
