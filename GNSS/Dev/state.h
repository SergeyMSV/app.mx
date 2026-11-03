#pragma once

#include <devConfig.h>

#include "config.h"
#include "dataset_hw.h"
#include "port_uart.h"

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

std::string ToString(tStatus a_status);

}
}
