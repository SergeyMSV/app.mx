#pragma once

#include <utilsBase.h>

#include <shareConfig.h>

#include <string>

#include <boost/property_tree/ptree.hpp>

namespace dev
{

namespace config
{

struct tEmail
{
	std::string To;
	uint32_t PeriodMinimum = 0;//in seconds, min limit set in c_tor - messages must not be sent more frequently

	tEmail() = default;
	explicit tEmail(const boost::property_tree::ptree& pTree);

	bool IsWrong() const;
};

struct tBoardRAM
{
	int32_t FreeDiff = 0;//in kB
	int32_t UsedDiff = 0;//in kB
	uint32_t Period = 0;//in seconds; min limit set in c_tor; period of measurement
	uint8_t Qty = 0;//items on the list

	tBoardRAM() = default;
	explicit tBoardRAM(const boost::property_tree::ptree& pTree);
};

}

class tDataSetConfig
{
	share_config::tDevice m_Device;
	config::tEmail m_Email;
	config::tBoardRAM m_BoardRAM;

public:
	tDataSetConfig(const std::string& fileNameConfig, const std::string& fileNameDevice, const std::string& fileNamePrivate);

	share_config::tDevice GetDevice() const { return m_Device; }
	config::tEmail GetEmail() const { return m_Email; }
	config::tBoardRAM GetBoardRAM() const { return m_BoardRAM; }
};

}