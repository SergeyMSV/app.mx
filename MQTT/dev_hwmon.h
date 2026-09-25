#pragma once

#include <string>
#include <vector>

namespace dev
{

struct tHwmon
{
	std::string ID;
	std::string Name;
	std::string Label;
	std::string Temperature;
	std::string Humidity;
	std::uint8_t Reg;
};

std::vector<tHwmon> GetHwmon();

}
