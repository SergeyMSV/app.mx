///////////////////////////////////////////////////////////////////////////////////////////////////
// utilsChrono.h
// 2021-12-29
// Standard ISO/IEC 114882, C++20
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <chrono>

namespace utils
{

using tClock = std::chrono::steady_clock;
using tTimePoint = std::chrono::time_point<tClock>;

using ttime_ns = std::chrono::nanoseconds;
using ttime_us = std::chrono::microseconds;
using ttime_ms = std::chrono::milliseconds;

template<class _Period, class T>
std::uint32_t GetDuration(std::chrono::time_point<T> timeStart, std::chrono::time_point<T> timeNow)
{
	if (timeStart >= timeNow)//[!]time can be changed backwards by OS (Windows)
		return 0;

	auto Duration = std::chrono::duration_cast<_Period>(timeNow - timeStart).count();
	return static_cast<std::uint32_t>(Duration);
}

class tTimePeriod
{
	uint32_t m_Period = 0;//in seconds
	const bool m_Sync = false;

	utils::tTimePoint m_StartTime = utils::tClock::now();

public:
	tTimePeriod() = delete;
	explicit tTimePeriod(bool sync)
		:m_Sync(sync)
	{}
	tTimePeriod(bool sync, uint32_t period)
		:m_Sync(sync), m_Period(period)
	{}

	bool Set(uint32_t period)
	{
		if (m_Period == period)
			return false;

		m_Period = period;
		m_StartTime = utils::tClock::now();
		return true;
	}

	bool IsReady()
	{
		auto TimeNow = utils::tClock::now();

		if (m_Period == 0 || m_StartTime > TimeNow)
			return false;

		if (m_Sync)
		{
			while (m_StartTime <= TimeNow)
			{
				m_StartTime += std::chrono::seconds(m_Period);
			}
		}
		else
		{
			m_StartTime = TimeNow + std::chrono::seconds(m_Period);
		}
		
		return true;
	}
};

}
