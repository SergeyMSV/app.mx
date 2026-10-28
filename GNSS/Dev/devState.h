#pragma once

#include <devConfig.h>

#include "dev/config.h"
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

std::pair<tDataSetHW, tStatus> Init(tPortUART& port, const tConfig& dsConfig);
tStatus Receive(tPortUART& port, const tConfig& dsConfig, const tDataSetHW& dsHW);

}
}
