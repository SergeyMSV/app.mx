#pragma once

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

}
}
