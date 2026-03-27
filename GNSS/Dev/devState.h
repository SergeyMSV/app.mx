#pragma once

#include <devConfig.h>

#include "devDataSetConfig.h"
#include "devDataSetHW.h"
#include "devPortUART.h"

namespace dev
{
namespace state
{

enum class tStatus
{
	None,
	InitSetBaudrate,
	InitNotSupported,
	InitNotResponded,
	//InitNotReceived,
	ReceiveNotSupportedChip,
	ReceiveNotSupportedModel,
	//NoDataReceived,

	Unknown = 0xFF,
};

std::pair<tDataSetHW, tStatus> Init(tPortUART& port, const tDataSetConfig& dsConfig);
tStatus Receive(tPortUART& port, const tDataSetConfig& dsConfig, const tDataSetHW& dsHW);

}
}
