#pragma once
#include "pch.h"
#include <time.h>
#include <atlstr.h>

namespace Util
{
class Timer
{
public:
	Timer()noexcept;
	void SetTime()noexcept;
	std::int64_t GetMilliseconds()const noexcept;
	double GetTime()const noexcept;

private:
	clock_t t;
};

#ifdef _WIN32
class HighResolutionTimer
{
public:
	HighResolutionTimer();
	void SetTime();
	std::int64_t GetMicroseconds()const;
	double GetTime()const;

private:
	Timer m_backup;//if there is any error, use this
	double m_clocks_cycle = 0;
	LARGE_INTEGER t;
};
#else
using HighResolutionTimer = Timer;
#endif
}