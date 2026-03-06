#include "devStateReceive.h"

namespace dev
{
namespace state
{

tStatus ReceiveMTK(tPortUART& port, const tDataSetHW& dsHW);
tStatus ReceiveSiRF(tPortUART& port, const tDataSetHW& dsHW);

tStatus Receive(tPortUART& port, const tDataSetHW& dsHW)
{
	if (dsHW.Chip == "SiRF")
		return ReceiveSiRF(port, dsHW);
	if (dsHW.Chip == "MTK")
		return ReceiveMTK(port, dsHW);

	// [TBD] generic

	return tStatus::ReceiveNotSupportedChip;
}

}
}
