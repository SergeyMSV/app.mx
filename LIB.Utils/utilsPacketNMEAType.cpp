#include "utilsPacketNMEAType.h"

namespace utils
{
namespace packet
{
namespace nmea
{
namespace type
{
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hidden
{
std::pair<std::uint32_t, std::uint32_t> SplitDouble(double value, std::size_t precision)
{
	const std::uint32_t Mult = static_cast<std::uint32_t>(std::pow(10, precision));
	const double Temp = std::round(value * Mult);
	const std::uint32_t ValInt = static_cast<std::uint32_t>(Temp / Mult);
	const std::uint32_t ValFract = static_cast<std::uint32_t>(Temp - ValInt * Mult);
	return { ValInt, ValFract };
}

double MakeDouble(std::int32_t valueInt, std::int32_t valueFract, std::size_t precision)
{
	const std::uint32_t Mult = static_cast<std::uint32_t>(std::pow(10, precision));
	double Val = static_cast<double>(valueFract) / Mult + std::abs(valueInt);
	if (valueInt < 0)
		Val *= -1;
	return Val;
}

int CountDigits(std::int32_t num)
{
	int Count = 0;
	while (num != 0)
	{
		num /= 10;
		++Count;
	}
	return Count;
}

bool IsInteger(const std::string& value)
{
	if (value.empty())
		return false;
	std::string::const_iterator Begin = value.cbegin();
	std::string::const_iterator End = value.cend();
	if (*Begin == '-')
		++Begin;
	for (std::string::const_iterator i = Begin; i != End; ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(*i)))
			return false;
	}
	return true;
}

bool CheckSignedIntFixed(const std::string& value, std::size_t size)
{
	if (value.empty())
		return false;
	if (value[0] == '-' && value.size() != size + 1)
		return false;
	if (value[0] != '-' && value.size() != size)
		return false;
	return hidden::IsInteger(value);
}

bool CheckSignedInt(const std::string& value, std::size_t sizeMax)
{
	if (value.empty())
		return false;
	if (value[0] == '-' && value.size() > sizeMax + 1)
		return false;
	if (value[0] != '-' && value.size() > sizeMax)
		return false;
	return hidden::IsInteger(value);
}

bool IsChar(char ch)
{
	return std::isalpha(static_cast<unsigned char>(ch));
}

}
///////////////////////////////////////////////////////////////////////////////////////////////////
tGNSS::tGNSS(const std::string& value)
{
	if (value.size() < 2 || value[0] != 'G')
	{
		SetVerified(false);
		return;
	}
	switch (value[1])
	{
	case 'P': Value = tGNSS_State::GPS; break;
	case 'L': Value = tGNSS_State::GLONASS; break;
	case 'N': Value = tGNSS_State::GlobalNavigation; break;
	default: SetVerified(false); return;
	}
}

std::string tGNSS::ToString() const
{
	switch (Value)
	{
	case tGNSS_State::GPS: return "GP";
	case tGNSS_State::GLONASS: return "GL";
	case tGNSS_State::GlobalNavigation: return "GN";
	default: return "--";
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
tStatus::tStatus(const std::string& value)
{
	if (value.size() != 1 || (value[0] != 'A' && value[0] != 'V'))
	{
		SetVerified(false);
		return;
	}
	Value = value[0] == 'A' ? true : false;
}

std::string tStatus::ToString() const
{
	if (!Value.has_value())
		return "";
	return *Value ? "A" : "V";
}
///////////////////////////////////////////////////////////////////////////////////////////////////
tDate::tDate(const std::string& value)
{
	if (value.empty())
		return;
	if (value.size() != 6)
	{
		ClearVerified();
		return;
	}
	m_Day = tValue(value.substr(0, 2));
	m_Month = tValue(value.substr(2, 2));
	m_Year = tValue(value.substr(4));
	if (!IsValid(m_Year.GetValue(), m_Month.GetValue(), m_Day.GetValue()))
		ClearVerified();
}

tDate::tDate(std::int8_t year, std::int8_t month, std::int8_t day)
{
	if (!IsValid(year, month, day))
	{
		ClearVerified();
		return;
	}
	m_Year = tValue(year);
	m_Month = tValue(month);
	m_Day = tValue(day);
}

tDate::tDate(std::time_t value)
{
	tm DateTime{};
#ifdef _WIN32
	localtime_s(&DateTime, &value);
#else // _WIN32
	localtime_r(&value, &DateTime);
#endif // _WIN32
	if (DateTime.tm_year > 100)
		DateTime.tm_year -= 100; // tm_year is from 1900
	m_Year = tValue(DateTime.tm_year);
	m_Month = tValue(DateTime.tm_mon + 1);
	m_Day = tValue(DateTime.tm_mday);
}

std::time_t tDate::GetValue() const
{
	if (IsEmpty())
		return {};
	tm DateTime{};
	DateTime.tm_year = m_Year.GetValue();
	if (DateTime.tm_year < 90) // since 1990
		DateTime.tm_year += 100; // tm_year is from 1900
	DateTime.tm_mon = m_Month.GetValue() - 1;
	DateTime.tm_mday = m_Day.GetValue();
	return mktime(&DateTime);
}

std::string tDate::ToString() const
{
	std::stringstream SStr;
	SStr << *this;
	return SStr.str();
}

void tDate::ClearVerified()
{
	m_Year = tValue(tTypeVerified::tStatus::False);
	m_Month = tValue(tTypeVerified::tStatus::False);
	m_Month = tValue(tTypeVerified::tStatus::False);
}

std::ostream& operator<<(std::ostream& out, const tDate& value)
{
	if (value.IsEmpty())
		return out;
	out << value.m_Day << value.m_Month << value.m_Year;
	return out;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
tMode::tMode(const std::string& value)
{
	if (value.empty())
		return;
	if (value.size() != 1 || value[0] < 'A' || value[0] > 'Z')
	{
		SetVerified(false);
		return;
	}
	m_Value = value[0];
}

std::string tMode::ToString() const
{
	std::stringstream SStr;
	SStr << *this;
	return SStr.str();
}

std::string tMode::ToStringEx() const
{
	switch (m_Value)
	{
	case 'A': return "Autonomous";
	case 'D': return "Differential";
	case 'E': return "Estimated";
	case 'M': return "Manual input";
	case 'S': return "Simulator";
	case 'N': return "Not valid";
	}
	return "Error";
}

std::ostream& operator<<(std::ostream& out, const tMode& value)
{
	if (value.IsEmpty())
		return out;
	out << value.GetValue();
	return out;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
tQuality::tQuality(const std::string& value)
{
	if (value.empty())
		return;
	if (value.size() != 1 || value[0] < '0' || value[0] > '8')
	{
		SetVerified(false);
		return;
	}
	m_Value = tValue(value);
}

tQuality::tQuality(value_type value)
{
	if (value > 8)
	{
		SetVerified(false);
		return;
	}
	m_Value = tValue(value);
}

std::string tQuality::ToString() const
{
	std::stringstream SStr;
	SStr << *this;
	return SStr.str();
}

std::string tQuality::ToStringEx() const
{
	if (!IsVerified())
		return "";
	switch (m_Value.GetValue())
	{
	case 0: return "Not valid";
	case 1: return "GPS SPS";
	case 2: return "Differential GPS, SPS";
	case 3: return "GPS PPS";
	case 4: return "Real Time Kinematic";
	case 5: return "Float RTK";
	case 6: return "Estimated";					// NMEA version 2.3 and later (Dead Reckoning)
	case 7: return "Manual input";
	case 8: return "Simulator";
	}
	return "";
}

std::ostream& operator<<(std::ostream& out, const tQuality& value)
{
	if (value.IsEmpty())
		return out;
	out << value.GetValue();
	return out;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
}
