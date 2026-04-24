///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsPacketNMEAType
// 2020-01-27, 2026-01-17 refact.
// C++17
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <algorithm>
#include <iomanip>
#include <ostream>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>

namespace utils
{
namespace packet
{
namespace nmea
{
namespace type
{
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hide
{

std::pair<std::uint32_t, std::uint32_t> SplitDouble(double value, std::size_t precision);
double MakeDouble(std::int32_t valueInt, std::int32_t valueFract, std::size_t precision);
int CountDigits(std::int32_t num);
bool IsInt(const std::string& value);
bool IsUInt(const std::string& value);
bool IsChar(char ch);

}
///////////////////////////////////////////////////////////////////////////////////////////////////
class tTypeVerified
{
	bool m_Verified = true;

public:
	enum class tStatus { False, True, };

	tTypeVerified() = default;
	explicit tTypeVerified(bool verified) :m_Verified(verified) {}
	explicit tTypeVerified(tStatus verified) :m_Verified(verified == tStatus::True) {}

	bool IsVerified() const { return m_Verified; }

protected:
	void SetVerified(bool verified) { m_Verified = verified; }
};

template<typename ... T>
bool IsVerified(const T& ... value)
{
	int Verified = true;
	auto fn = [&Verified](bool verif)
		{
			if (!verif)
				Verified = false;
			return Verified;
		};
	(fn(value.IsVerified()) && ...);
	return Verified;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hide
{
template<typename T>
class tTypeNoNull
{
	template<typename S>
	friend std::ostream& operator<<(std::ostream& out, const tTypeNoNull<S>& value);

	T m_Value;

public:
	using value_type = typename T::value_type;

	tTypeNoNull() = default;
	explicit tTypeNoNull(tTypeVerified::tStatus verified) :m_Value(verified) {}
	explicit tTypeNoNull(const std::string& value) :m_Value(value) {}
	tTypeNoNull(const std::string& value, const std::string& sign) :m_Value(value, sign) {}
	explicit tTypeNoNull(value_type value) :m_Value(value) {}

	bool IsVerified() const { return m_Value.IsVerified() && !m_Value.IsEmpty(); }

	bool IsEmpty() const { return m_Value.IsEmpty(); }

	static constexpr std::size_t GetSize() { return T::GetSize(); }

	value_type GetValue() const { return m_Value.GetValue(); }

	std::string ToString() const { return m_Value.ToString(); }
	std::string ToStringEx() const { return m_Value.ToStringEx(); } // [TBD] That is not perfect.
	std::string ToStringValue() const { return m_Value.ToStringValue(); } // [TBD] That is not perfect.
	std::string ToStringHemisphere() const { return m_Value.ToStringHemisphere(); } // [TBD] That is not perfect.
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const tTypeNoNull<T>& value)
{
	out << value.m_Value;
	return out;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////
enum class tGNSS_State : std::uint8_t // It's like bitfield.
{
	None = 0,
	GPS = 1,			// GP	0000'0001
	GLONASS,			// GL	0000'0010
	//Galileo,			// GA
	//QZSS				// GQ
	//BeiDou			// BD
	GlobalNavigation,	// GN	0000'0011		If a solution is obtained and combined from multiple systems.
};
///////////////////////////////////////////////////////////////////////////////////////////////////
struct tGNSS : public tTypeVerified
{
	tGNSS_State Value = tGNSS_State::None;

	tGNSS() :tTypeVerified(false) {}
	explicit tGNSS(tGNSS_State value) : Value(value) {}
	explicit tGNSS(const std::string& value);

	bool IsEmpty() const { return Value == tGNSS_State::None; }

	int GetValue() const { return static_cast<int>(Value); }

	std::string ToString() const;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
class tStatus : public tTypeVerified
{
	std::optional<bool> Value;

public:
	using value_type = bool;

	tStatus() :tTypeVerified(false) {}
	explicit tStatus(value_type value) : Value(value) {}
	explicit tStatus(const std::string& value);

	bool IsEmpty() const { return !Value.has_value(); }

	value_type GetValue() const { return Value.value_or(false); }

	std::string ToString() const;
};

using tStatusNoNull = hide::tTypeNoNull<tStatus>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t Size>
class tUIntFixed : public tTypeVerified
{
	static_assert(Size > 0, "Size must be greater than 0.");

	template<std::size_t S>
	friend std::ostream& operator<<(std::ostream& out, const tUIntFixed<S>& value);

	std::optional<std::int32_t> m_Value;

public:
	using value_type = std::int32_t;

	tUIntFixed() = default;
	explicit tUIntFixed(tTypeVerified::tStatus verified) :tTypeVerified(verified) {}
	explicit tUIntFixed(const std::string& value)
	{
		if (value.empty())
			return;
		if (value.size() == Size)
		{
			std::int32_t Value = std::strtol(value.c_str(), nullptr, 10);
			if (Value >= 0)
			{
				m_Value = Value;
				return;
			}
		}
		SetVerified(false);
	}
	explicit tUIntFixed(std::int32_t value)
	{
		if (value < 0 || hide::CountDigits(value) > Size)
		{
			SetVerified(false);
			return;
		}
		m_Value = value;
	}

	static constexpr std::size_t GetSize() { return Size; }

	bool IsEmpty() const { return !m_Value.has_value(); }

	std::int32_t GetValue() const { return m_Value.value_or(0); }

	std::string ToString() const
	{
		std::stringstream SStr;
		SStr << *this;
		return SStr.str();
	}
};

template<std::size_t Size>
std::ostream& operator<<(std::ostream& out, const tUIntFixed<Size>& value)
{
	if (value.IsEmpty())
		return out;
	std::int32_t Val = value.GetValue();
	out << std::setfill('0') << std::setw(Size) << Val;
	return out;
}

template <std::size_t Size>
using tUIntFixedNoNull = hide::tTypeNoNull<tUIntFixed<Size>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hide
{
template <std::size_t Size>
class tIntFixedBase : public tTypeVerified
{
	template<std::size_t S>
	friend std::ostream& operator<<(std::ostream& out, const tIntFixedBase<S>& value);

protected:
	std::optional<std::int32_t> m_Value;

public:
	using value_type = std::int32_t;

	tIntFixedBase() = default;
	explicit tIntFixedBase(tTypeVerified::tStatus verified) :tTypeVerified(verified) {}

	bool IsEmpty() const { return !m_Value.has_value(); }

	std::int32_t GetValue() const { return m_Value.value_or(0); }

	std::string ToString() const
	{
		std::stringstream SStr;
		SStr << *this;
		return SStr.str();
	}
};

template<std::size_t Size>
std::ostream& operator<<(std::ostream& out, const tIntFixedBase<Size>& value)
{
	if (value.IsEmpty())
		return out;
	std::size_t ValSize = Size;
	std::int32_t Val = value.GetValue();
	if (Val < 0)
	{
		out << '-';
		--ValSize;
	}
	out << std::setfill('0') << std::setw(ValSize) << std::abs(Val);
	//out << std::setfill('0') << std::setw(Size) << value.GetValue(); // 0000-1.34
	return out;
}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t Size>
class tIntFixedStrict : public hide::tIntFixedBase<Size> // Example: 000, 012, 123, -02, -12 for Size = 3
{
	using tBase = hide::tIntFixedBase<Size>;

	static_assert(Size > 1, "Size must be greater than 1.");

public:
	tIntFixedStrict() = default;
	explicit tIntFixedStrict(tTypeVerified::tStatus verified) :tBase(verified) {}
	explicit tIntFixedStrict(const std::string& value)
	{
		if (value.empty())
			return;
		if (value.size() != Size || !hide::IsInt(value))
		{
			tBase::SetVerified(false);
			return;
		}
		tBase::m_Value = std::strtol(value.c_str(), nullptr, 10);
	}
	explicit tIntFixedStrict(std::int32_t value)
	{
		if (hide::CountDigits(value) > Size)
		{
			tBase::SetVerified(false);
			return;
		}
		tBase::m_Value = value;
	}

	static constexpr std::size_t GetSize() { return Size; }
};

template <std::size_t Size>
using tIntFixedStrictNoNull = hide::tTypeNoNull<tIntFixedStrict<Size>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t Size>
class tIntFixed : public hide::tIntFixedBase<Size> // Example: 000, 012, 123, -02, -12, -123  for Size = 3. It can't be unsigned.
{
	using tBase = hide::tIntFixedBase<Size>;

	static_assert(Size > 0, "Size must be greater than 0.");

public:
	tIntFixed() = default;
	explicit tIntFixed(tTypeVerified::tStatus verified) :tBase(verified) {}
	explicit tIntFixed(const std::string& value)
	{
		assert(Size >= 1);
		if (value.empty())
			return;
		const std::size_t SizeReal = value.size() > Size && value[0] == '-' && value[1] != '0' ? Size + 1 : Size; // -01, -12, -123
		if (!hide::IsInt(value) || value.size() != SizeReal)
		{
			tBase::SetVerified(false);
			return;
		}
		tBase::m_Value = std::strtol(value.c_str(), nullptr, 10);
	}
	explicit tIntFixed(std::int32_t value)
	{
		const std::size_t SizeMax = value < 0 ? Size + 1 : Size;
		if (hide::CountDigits(value) > SizeMax)
		{
			tBase::SetVerified(false);
			return;
		}
		tBase::m_Value = value;
	}

	//static constexpr std::size_t GetSize() { return Size; } // [TBD] that's wrong, the size depends on sign and quantity of digits
};

template <std::size_t Size>
using tIntFixedNoNull = hide::tTypeNoNull<tIntFixed<Size>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t SizeMax>
class tUInt : public tTypeVerified // It can consist of any quantity of digits from 1 upto Size.
{
	static_assert(SizeMax > 0, "Size must be greater than 0.");

	template <std::size_t S>
	friend std::ostream& operator<<(std::ostream& out, const tUInt<S>& value);

	std::optional<std::int32_t> m_Value;

public:
	using value_type = std::int32_t;

	tUInt() = default;
	explicit tUInt(tTypeVerified::tStatus verified) :tTypeVerified(verified) {}
	explicit tUInt(const std::string& value)
	{
		if (value.empty() )
			return;
		if (!hide::IsUInt(value) || value.size() > SizeMax)
		{
			SetVerified(false);
			return;
		}
		m_Value = std::strtol(value.c_str(), nullptr, 10);
	}
	explicit tUInt(std::int32_t value)
	{
		if (value < 0 || hide::CountDigits(value) > SizeMax)
		{
			SetVerified(false);
			return;
		}
		m_Value = value;
	}

	static constexpr std::size_t GetSize() { return SizeMax; }

	bool IsEmpty() const { return !m_Value.has_value(); }

	std::int32_t GetValue() const { return m_Value.value_or(0); }

	std::string ToString() const
	{
		std::stringstream SStr;
		SStr << *this;
		return SStr.str();
	}
};

template <std::size_t SizeMax>
std::ostream& operator<<(std::ostream& out, const tUInt<SizeMax>& value)
{
	if (value.IsEmpty())
		return out;
	out << *value.m_Value;
	return out;
}

template <std::size_t SizeMax>
using tUIntNoNull = hide::tTypeNoNull<tUInt<SizeMax>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t SizeMax>
class tInt : public tTypeVerified // It can consist of any quantity of digits from 1 upto Size.
{
	static_assert(SizeMax > 1, "Size must be greater than 1.");

	template <std::size_t S>
	friend std::ostream& operator<<(std::ostream& out, const tInt<S>& value);

	std::optional<std::int32_t> m_Value;

public:
	using value_type = std::int32_t;

	tInt() = default;
	explicit tInt(tTypeVerified::tStatus verified) :tTypeVerified(verified) {}
	explicit tInt(const std::string& value)
	{
		if (value.empty())
			return;
		if (!hide::IsInt(value) || value.size() > SizeMax)
		{
			SetVerified(false);
			return;
		}
		m_Value = std::strtol(value.c_str(), nullptr, 10);
	}
	explicit tInt(std::uint32_t value)
	{
		if (hide::CountDigits(value) > SizeMax)
		{
			SetVerified(false);
			return;
		}
		m_Value = value;
	}

	static constexpr std::size_t GetSize() { return SizeMax; } 

	bool IsEmpty() const { return !m_Value.has_value(); }

	std::int32_t GetValue() const { return m_Value.value_or(0); }

	std::string ToString() const
	{
		std::stringstream SStr;
		SStr << *this;
		return SStr.str();
	}
};

template <std::size_t SizeMax>
std::ostream& operator<<(std::ostream& out, const tInt<SizeMax>& value)
{
	if (value.IsEmpty())
		return out;
	out << *value.m_Value;
	return out;
}

template <std::size_t SizeMax>
using tIntNoNull = hide::tTypeNoNull<tInt<SizeMax>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hide
{

template <typename TInt, typename TFract>
class tFloatBase : public tTypeVerified
{
	using tValueInt = tTypeNoNull<TInt>;
	using tValueFract = tTypeNoNull<TFract>;
	using tValue = std::pair<tValueInt, tValueFract>;

	template<typename S, typename P>
	friend std::ostream& operator<<(std::ostream& out, const tFloatBase<S, P>& value);
	
	std::optional<tValue> m_Value;

public:
	using value_type = double;

	tFloatBase() = default;
	explicit tFloatBase(tTypeVerified::tStatus verified) :tTypeVerified(verified) {}
	explicit tFloatBase(const std::string& value)
	{
		const std::size_t DotPos = value.find('.');
		if (DotPos == std::string::npos)
			return;
		m_Value = { tValueInt(value.substr(0, DotPos)), tValueFract(value.substr(DotPos + 1)) };
	}
	explicit tFloatBase(double value)
	{
		std::pair<std::int32_t, std::int32_t> Data = hide::SplitDouble(std::abs(value), tValueFract::GetSize());
		if (value < 0)
			Data.first *= -1;
		m_Value = { tValueInt(Data.first), tValueFract(Data.second) };
	}

	static constexpr std::size_t GetSize() { return tValueInt::GetSize() + 1 + tValueFract::GetSize(); }

	bool IsVerified() const
	{
		if (!tTypeVerified::IsVerified())
			return false;
		if (!m_Value.has_value())
			return true;
		return m_Value->first.IsVerified() && m_Value->second.IsVerified();
	}

	bool IsEmpty() const
	{
		if (!m_Value.has_value())
			return true;
		return m_Value->first.IsEmpty() || m_Value->second.IsEmpty();
	}

	double GetValue() const
	{
		if (!m_Value.has_value())
			return 0;
		return hide::MakeDouble(m_Value->first.GetValue(), m_Value->second.GetValue(), tValueFract::GetSize());
	}

	std::string ToString() const
	{
		std::stringstream SStr;
		SStr << *this;
		return SStr.str();
	}
};

template <typename TInt, typename TFract>
std::ostream& operator<<(std::ostream& out, const tFloatBase<TInt, TFract>& value)
{
	if (value.IsEmpty())
		return out;
	out << value.m_Value->first << '.' << value.m_Value->second;
	return out;
}

}
///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
class tUnit
{
	template<typename U>
	friend std::ostream& operator<<(std::ostream& out, const tUnit<U>& value);

protected:
	T m_Value;
	char m_Unit = 0;

public:
	using value_type = typename T::value_type;

	tUnit() = default;
	tUnit(const std::string& value, const std::string& unit)
	{
		m_Value = T(value);
		if (unit.size() == 1 && hide::IsChar(unit[0]))
			m_Unit = unit[0]; // It should be parsed in any case even if Value is null.
	}
	tUnit(value_type value, char unit)
	{
		m_Value = T(value);
		if (hide::IsChar(unit))
			m_Unit = unit;
	}

	bool IsVerified() const { return m_Value.IsVerified() && (!m_Unit || hide::IsChar(m_Unit)); }

	bool IsEmpty() const { return m_Value.IsEmpty() && !hide::IsChar(m_Unit); }

	static constexpr std::size_t GetSize() { return 0; }

	value_type GetValue() const { return m_Value.GetValue(); }

	std::string ToString() const
	{
		std::stringstream SStr;
		SStr << *this;
		return SStr.str();
	}

	std::string ToStringValue() const
	{
		std::stringstream SStr;
		SStr << m_Value;
		return SStr.str();
	}

	std::string ToStringUnit() const
	{
		if (!hide::IsChar(m_Unit))
			return {};
		return std::string(1, m_Unit);
	}
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const tUnit<T>& value)
{
	out << value.m_Value << ',';
	if (hide::IsChar(value.m_Unit))
		out << value.m_Unit;
	return out;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
template<typename T>
class tUnitNoNull : public tUnit<T>
{
	using tBase = tUnit<T>;

public:
	using value_type = typename T::value_type;

	tUnitNoNull() = default;
	tUnitNoNull(const std::string& value, const std::string& unit) :tBase(value, unit) {}
	tUnitNoNull(value_type value, char unit) :tBase(value, unit) {}

	bool IsVerified() const { return this->m_Value.IsVerified() && hide::IsChar(this->m_Unit); }

	bool IsEmpty() const { return this->m_Value.IsEmpty(); }
};
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t SizeInt, std::size_t Precision>
using tFloatFixedStrict = hide::tFloatBase<tIntFixedStrict<SizeInt>, tUIntFixed<Precision>>;

template <std::size_t SizeInt, std::size_t Precision>
using tFloatFixedStrictNoNull = hide::tTypeNoNull<tFloatFixedStrict<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tFloatFixedStrictUnit = tUnit<tFloatFixedStrict<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tFloatFixedStrictUnitNoNull = tUnitNoNull<tFloatFixedStrict<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tFloatFixedStrictNoNullUnit = tUnit<tFloatFixedStrictNoNull<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tFloatFixedStrictNoNullUnitNoNull = tUnitNoNull<tFloatFixedStrictNoNull<SizeInt, Precision>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t SizeInt, std::size_t Precision>
using tFloatFixed = hide::tFloatBase<tIntFixed<SizeInt>, tUIntFixed<Precision>>;

template <std::size_t SizeInt, std::size_t Precision>
using tFloatFixedNoNull = hide::tTypeNoNull<tFloatFixed<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tFloatFixedUnit = tUnit<tFloatFixed<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tFloatFixedUnitNoNull = tUnitNoNull<tFloatFixed<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tFloatFixedNoNullUnit = tUnit<tFloatFixedNoNull<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tFloatFixedNoNullUnitNoNull = tUnitNoNull<tFloatFixedNoNull<SizeInt, Precision>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t SizeInt, std::size_t Precision>
using tUFloatFixed = hide::tFloatBase<tUIntFixed<SizeInt>, tUIntFixed<Precision>>;

template <std::size_t SizeInt, std::size_t Precision>
using tUFloatFixedNoNull = hide::tTypeNoNull<tUFloatFixed<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tUFloatFixedUnit = tUnit<tUFloatFixed<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tUFloatFixedUnitNoNull = tUnitNoNull<tUFloatFixed<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tUFloatFixedNoNullUnit = tUnit<tUFloatFixedNoNull<SizeInt, Precision>>;

template<std::size_t SizeInt, std::size_t Precision>
using tUFloatFixedNoNullUnitNoNull = tUnitNoNull<tUFloatFixedNoNull<SizeInt, Precision>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t SizeIntMax, std::size_t Precision>
using tFloat = hide::tFloatBase<tInt<SizeIntMax>, tUInt<Precision>>;

template <std::size_t SizeIntMax, std::size_t PrecisionMax>
using tFloatNoNull = hide::tTypeNoNull<tFloat<SizeIntMax, PrecisionMax>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template<std::size_t SizeIntMax, std::size_t Precision>
using tFloatPrecisionFixed = hide::tFloatBase<tInt<SizeIntMax>, tUIntFixed<Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tFloatPrecisionFixedNoNull = hide::tTypeNoNull<tFloatPrecisionFixed<SizeIntMax, Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tFloatPrecisionFixedUnit = tUnit<tFloatPrecisionFixed<SizeIntMax, Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tFloatPrecisionFixedUnitNoNull = tUnitNoNull<tFloatPrecisionFixed<SizeIntMax, Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tFloatPrecisionFixedNoNullUnit = tUnit<tFloatPrecisionFixedNoNull<SizeIntMax, Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tFloatPrecisionFixedNoNullUnitNoNull = tUnitNoNull<tFloatPrecisionFixedNoNull<SizeIntMax, Precision>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t SizeIntMax, std::size_t PrecisionMax>
using tUFloat = hide::tFloatBase<tUInt<SizeIntMax>, tUInt<PrecisionMax>>;

template <std::size_t SizeIntMax, std::size_t PrecisionMax>
using tUFloatNoNull = hide::tTypeNoNull<tUFloat<SizeIntMax, PrecisionMax>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template<std::size_t SizeIntMax, std::size_t Precision>
using tUFloatPrecisionFixed = hide::tFloatBase<tUInt<SizeIntMax>, tUIntFixed<Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tUFloatPrecisionFixedNoNull = hide::tTypeNoNull<tUFloatPrecisionFixed<SizeIntMax, Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tUFloatPrecisionFixedUnit = tUnit<tUFloatPrecisionFixed<SizeIntMax, Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tUFloatPrecisionFixedUnitNoNull = tUnitNoNull<tUFloatPrecisionFixed<SizeIntMax, Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tUFloatPrecisionFixedNoNullUnit = tUnit<tUFloatPrecisionFixedNoNull<SizeIntMax, Precision>>;

template<std::size_t SizeIntMax, std::size_t Precision>
using tUFloatPrecisionFixedNoNullUnitNoNull = tUnitNoNull<tUFloatPrecisionFixedNoNull<SizeIntMax, Precision>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
class tDate
{
	using tValue = tUIntFixed<2>;

	friend std::ostream& operator<<(std::ostream& out, const tDate& value);

	tValue m_Day;
	tValue m_Month;
	tValue m_Year;

public:
	using value_type = std::time_t;

	tDate() = default;
	explicit tDate(tTypeVerified::tStatus verified) :m_Day(verified), m_Month(verified), m_Year(verified) {}
	explicit tDate(const std::string& value);
	tDate(std::int8_t year, std::int8_t month, std::int8_t day);
	explicit tDate(std::time_t value);

	bool IsVerified() const { return m_Day.IsVerified() && m_Month.IsVerified() && m_Year.IsVerified(); }

	bool IsEmpty() const { return m_Day.IsEmpty() || m_Month.IsEmpty() || m_Year.IsEmpty(); }

	std::time_t GetValue() const;

	std::uint8_t GetYear() const { return static_cast<std::uint8_t>(m_Day.GetValue()); }
	std::uint8_t GetMonth() const { return static_cast<std::uint8_t>(m_Month.GetValue()); }
	std::uint8_t GetDay() const { return static_cast<std::uint8_t>(m_Year.GetValue()); }

	std::string ToString() const;

private:
	void ClearVerified();
	static bool IsValid(std::uint8_t year, std::uint8_t month, std::uint8_t day) { return year < 100 && month > 0 && month <= 12 && day > 0 && day <= 31; }
};

std::ostream& operator<<(std::ostream& out, const tDate& value);

using tDateNoNull = hide::tTypeNoNull<tDate>;
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace hide
{

template<typename TSecond>
class tTimeBase
{
	using tValue = tUIntFixed<2>;

	template<typename S>
	friend std::ostream& operator<<(std::ostream& out, const tTimeBase<S>& value);

	tValue m_Hour;
	tValue m_Minute;
	TSecond m_Second;

public:
	using value_type = double;

	tTimeBase() = default;
	explicit tTimeBase(tTypeVerified::tStatus verified) :m_Hour(verified), m_Minute(verified), m_Second(verified) {}
	explicit tTimeBase(const std::string& value)
	{
		if (value.empty())
			return;
		if (value.size() != 4 + TSecond::GetSize())
		{
			ClearVerified();
			return;
		}
		m_Hour = tValue(value.substr(0, 2));
		m_Minute = tValue(value.substr(2, 2));
		m_Second = TSecond(value.substr(4));
		if (!IsValid(m_Hour.GetValue(), m_Minute.GetValue(), static_cast<std::uint8_t>(m_Second.GetValue())))
			ClearVerified();
	}
	tTimeBase(std::uint8_t hour, std::uint8_t minute, double second)
	{
		if (!IsValid(hour, minute, static_cast<std::uint8_t>(second)))
		{
			ClearVerified();
			return;
		}
		m_Hour = tValue(hour);
		m_Minute = tValue(minute);
		m_Second = TSecond(static_cast<typename TSecond::value_type>(second));
	}
	explicit tTimeBase(value_type value) // value is in seconds
	{
		m_Hour = tValue(value / 3600);
		value -= m_Hour.GetValue() * 3600;
		m_Minute = tValue(value / 60);
		value -= m_Minute.GetValue() * 60;
		m_Second = TSecond(value);
	}

	bool IsVerified() const { return m_Hour.IsVerified() && m_Minute.IsVerified() && m_Second.IsVerified(); }

	bool IsEmpty() const { return m_Hour.IsEmpty() || m_Minute.IsEmpty() || m_Second.IsEmpty(); }

	value_type GetValue() const // in seconds
	{
		return m_Hour.GetValue() * 3600 + m_Minute.GetValue() * 60 + m_Second.GetValue();
	}

	std::uint8_t GetHour() const { return static_cast<std::uint8_t>(m_Hour.GetValue()); }
	std::uint8_t GetMinute() const { return static_cast<std::uint8_t>(m_Minute.GetValue()); }
	double GetSecond() const { return m_Second.GetValue(); }

	std::string ToString() const
	{
		std::stringstream SStr;
		SStr << *this;
		return SStr.str();
	}

private:
	void ClearVerified()
	{
		m_Hour = tValue(tTypeVerified::tStatus::False);
		m_Minute = tValue(tTypeVerified::tStatus::False);
		m_Second = TSecond(tTypeVerified::tStatus::False);
	}
	static bool IsValid(std::uint8_t hour, std::uint8_t minute, std::uint8_t second) { return hour < 24 && minute < 60 && second < 60; }
};

template <typename TSecond>
std::ostream& operator<<(std::ostream& out, const tTimeBase<TSecond>& value)
{
	if (value.IsEmpty())
		return out;
	out << value.m_Hour << value.m_Minute << value.m_Second;
	return out;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
template<std::size_t Precision>
struct tTimeHelper
{
	using type = tTimeBase<tUFloatFixed<2, Precision>>;
};

template<>
struct tTimeHelper<0>
{
	using type = tTimeBase<tUIntFixed<2>>;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
}
///////////////////////////////////////////////////////////////////////////////////////////////////
template<std::size_t Precision>
using tTime = typename hide::tTimeHelper<Precision>::type;

template<std::size_t Precision>
using tTimeNoNull = hide::tTypeNoNull<tTime<Precision>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
// The valid range of latitude in degrees is -90 and +90 for the southern and northern hemisphere, respectively.
// Longitude is in the range -180 and +180 specifying coordinates west and east of the Prime Meridian, respectively. For reference,
// the Equator has a latitude of 0, the North pole has a latitude of 90 north(written 90 N or +90), and the South pole has a latitude of -90.
template <std::size_t SizeDeg, std::size_t Precision>
class tGeoDegree
{
	using tDegree = tUIntFixed<SizeDeg>;
	using tMinute = tUFloatFixed<2, Precision>;

	static constexpr std::size_t Size = tDegree::GetSize() + tMinute::GetSize();
	static constexpr int MaxAbs = SizeDeg == 3 ? 180 : 90;

	template<std::size_t S, std::size_t P>
	friend std::ostream& operator<<(std::ostream& out, const tGeoDegree<S, P>& value);

	tDegree m_Deg;
	tMinute m_Min;
	bool m_Negative = false;

public:
	using value_type = double;

	static constexpr char SignNegative = SizeDeg == 3 ? 'W' : 'S';
	static constexpr char SignPositive = SizeDeg == 3 ? 'E' : 'N';

	tGeoDegree() = default;
	tGeoDegree(const std::string& value, const std::string& sign)
	{
		if (value.empty() && sign.empty())
			return;
		if (value.size() != Size || sign.size() != 1 || (sign[0] != SignNegative && sign[0] != SignPositive))
		{
			ClearVerified();
			return;
		}
		m_Negative = sign[0] == SignNegative;
		m_Deg = tDegree(value.substr(0, SizeDeg));
		m_Min = tMinute(value.substr(SizeDeg));
		if (!IsValid(GetValue()))
			ClearVerified();
	}
	explicit tGeoDegree(value_type degree)
	{
		if (!IsValid(degree))
		{
			ClearVerified();
			return;
		}
		m_Negative = degree < 0;
		degree = std::abs(degree);
		m_Deg = tDegree(static_cast<std::int32_t>(degree));
		degree -= static_cast<std::int32_t>(degree);
		degree *= 60;
		m_Min = tMinute(degree);
	}

	bool IsVerified() const { return m_Deg.IsVerified() && m_Min.IsVerified(); }

	bool IsEmpty() const { return m_Deg.IsEmpty() || m_Min.IsEmpty(); }

	static constexpr std::size_t GetSize() { return Size; }

	value_type GetValue() const { return m_Deg.GetValue() + m_Min.GetValue() / 60; }

	std::string ToString() const
	{
		std::stringstream SStr;
		SStr << *this;
		return SStr.str();
	}

	std::string ToStringValue() const
	{
		std::stringstream SStr;
		SStr << m_Deg << m_Min;
		return SStr.str();
	}

	std::string ToStringHemisphere() const
	{
		if (IsEmpty())
			return {};
		return std::string(1, m_Negative ? SignNegative : SignPositive);
	}

private:
	void ClearVerified()
	{
		m_Deg = tDegree(tTypeVerified::tStatus::False);
		m_Min = tMinute(tTypeVerified::tStatus::False);
	}
	static bool IsValid(double value) { return value >= -MaxAbs && value <= MaxAbs; }
};

template <std::size_t SizeDeg, std::size_t Precision>
std::ostream& operator<<(std::ostream& out, const tGeoDegree<SizeDeg, Precision>& value)
{
	using tDeg = tGeoDegree<SizeDeg, Precision>;

	if (value.IsEmpty())
	{
		out << ',';
		return out;
	}
	out << value.m_Deg << value.m_Min << ',' << (value.m_Negative ? tDeg::SignNegative : tDeg::SignPositive);
	return out;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t Precision>
using tLatitude = tGeoDegree<2, Precision>;

template <std::size_t Precision>
using tLatitudeNoNull = hide::tTypeNoNull<tLatitude<Precision>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
template <std::size_t Precision>
using tLongitude = tGeoDegree<3, Precision>;

template <std::size_t Precision>
using tLongitudeNoNull = hide::tTypeNoNull<tLongitude<Precision>>;
///////////////////////////////////////////////////////////////////////////////////////////////////
class tMode : public tTypeVerified // NMEA version 2.3 and later
{
	char m_Value = 0;

public:
	using value_type = char;

	tMode() = default;
	explicit tMode(const std::string& value);

	bool IsEmpty() const { return m_Value == 0; }

	value_type GetValue() const { return m_Value; }

	std::string ToString() const;
	std::string ToStringEx() const;
};

std::ostream& operator<<(std::ostream& out, const tMode& value);

using tModeNoNull = hide::tTypeNoNull<tMode>;
///////////////////////////////////////////////////////////////////////////////////////////////////
class tQuality : public tTypeVerified
{
	using tValue = tUIntFixed<1>;

	tValue m_Value;

public:
	using value_type = std::uint32_t;

	tQuality() = default;
	explicit tQuality(const std::string& value);
	explicit tQuality(value_type value);

	bool IsVerified() const { return tTypeVerified::IsVerified() && m_Value.IsVerified(); }

	bool IsEmpty() const { return m_Value.IsEmpty(); }

	value_type GetValue() const { return m_Value.GetValue(); }

	std::string ToString() const;
	std::string ToStringEx() const;
};

std::ostream& operator<<(std::ostream& out, const tQuality& value);

using tQualityNoNull = hide::tTypeNoNull<tQuality>;
///////////////////////////////////////////////////////////////////////////////////////////////////
}
}
}
}
