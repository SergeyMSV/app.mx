///////////////////////////////////////////////////////////////////////////////////////////////////
// devLog
// 2020-01-15
// C++14
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <utilsLog.h>

#include <cstdint>

namespace dev
{

class tLog : public utils::log::tLog
{
public:
	enum class tID : std::uint32_t
	{
		NoID = 0,
		Enabled = (1 << 0),
		GNSS = (1 << 1),
	};

	union tSettings
	{
		struct
		{
			std::uint32_t Enabled : 1;//if it's equal to 0 no logs are shown
			std::uint32_t GNSS    : 1;
			std::uint32_t         : 29;
		}Field;

		std::uint32_t Value = 0;
	};

	static tSettings LogSettings;

private:
	const tID m_ID;
	const std::string m_Label;

	tLog() = delete;

public:
	tLog(tLog::tID id, const std::string& label);

protected:
	std::string GetLabel() const override { return m_Label; }

	void WriteLog(const std::string& msg) override;
};

}
