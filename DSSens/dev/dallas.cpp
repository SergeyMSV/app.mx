#include "dallas.h"

#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

#include <utilsDALLAS.h>

#include <sharePortHolder.h>
#include <shareUtilsFile.h>

namespace dallas = utils::dallas;

namespace dev
{

static std::string ToString(const dallas::tID& id)
{
	std::stringstream SStr;
	std::for_each(std::rbegin(id.Value), std::rend(id.Value), [&SStr](std::uint8_t val) { SStr << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(val); });
	return SStr.str();
}

void ThreadDALLAS(const tConfig& a_config)
{
	using tPortHld = tPortHolder<dallas::tDALLAS>;

	try
	{
		const share::config::port::tUART_Config portConfig = a_config.GetDallas();
		if (portConfig.ID.empty())
			return; // [TBD] throw an exception

		tPortHld portPtr = tPortHld(portConfig.ID, dallas::tDALLAS::tSpeed::Norm);
		if (!portPtr().IsReady())
			return; // [TBD] throw an exception --- or wait for a minute...

		std::set<dallas::tROM> roms;

		while (true)
		{
			{
				std::vector<dallas::tROM> romsFound = portPtr().Search(dallas::tFamilyCode::DS18B20);
				for (auto& rom : romsFound)
				{
					roms.insert(rom);
				}
			}

			if (roms.empty())
			{
				std::this_thread::sleep_for(std::chrono::seconds(10));
				continue;
			}

			for (int i = 0; i < 60; ++i) // for 1 minute
			{
				std::vector<dallas::tDsDS18B20> temps = portPtr().GetDsDS18B20(roms);
				if (!temps.empty())
				{
					std::string json = "[";
					for (auto& temp : temps)
					{
						if (json.size() > 1)
							json += ',';
						std::string romID = ToString(temp.ROM.Field.ID);
						config::tSensorProp sensProp = a_config.GetSensorProp(romID);
						json += "{\"id\":\"" + sensProp.ID + "\",\"name\":\"ds18b20\",\"label\":\"" + sensProp.Label + "\",\"temp1_input\":\"" + std::to_string(temp.Temperature) + "\"}";
					}
					json += "]\n";
#ifdef DSSENS_DALLAS_TEST
					std::cout << json;
#else // DSSENS_DALLAS_TEST
					share::SaveOutFile(a_config.GetOutFile(), json);
#endif // DSSENS_DALLAS_TEST
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}

}
