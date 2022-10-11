#pragma once

#include "shareConfig.h"

#include <string>

namespace share
{

enum class tHTMLFieldStatus
{
	Neutral,
	OK,
	Norm,			//bgcolor="lightblue"
	NormAgain,		//bgcolor="lightgreen"
	Warn,			//bgcolor="pink"
	Super,			//bgcolor="cyan"
	TableHeader,	//bgcolor="lightgray"
};

std::string GetHTMLBgColour(tHTMLFieldStatus status);

std::string GetHTMLTableDevice(const std::string& styleCol1);
std::string GetHTMLTableSystem(const std::string& styleCol1, const share_config::tDevice& dsConfigDevice);

}
