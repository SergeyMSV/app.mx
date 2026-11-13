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
	InitFailed,
	//InitNotReceived,
	ReceiveNotSupportedChip,
	ReceiveNotSupportedModel,
	//NoDataReceived,

	RestartDetected,

	Unknown = 0xFF,
};

std::pair<tDataSetHW, tStatus> Init(tPortUART& port, const tConfig& dsConfig);
tStatus Receive(tPortUART& a_port, const tConfig& a_dsConfig, const tDataSetHW& a_dsHW, tLog& a_log);

std::string ToString(tStatus a_status);

}
}
