#pragma once
#include <chrono>

namespace Util
{
template <typename T>
requires std::chrono::is_clock_v<T>
class _Timer
{
protected:
	std::chrono::time_point<T> t;

public:
	_Timer() :t( T::now() )	{}
	void SetTime()
	{
		t = T::now();
	}
	double GetSeconds()const
	{
		return std::chrono::duration_cast<std::chrono::duration<double>>( T::now() - t ).count();
	}
	std::int64_t GetMilliseconds()const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>( T::now() - t ).count();
	}
	std::int64_t GetMicroseconds()const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>( T::now() - t ).count();
	}
	std::int64_t GetNanoseconds()const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>( T::now() - t ).count();
	}
	double GetTime()const
	{
		return GetSeconds();
	}
};

using Timer = _Timer<std::chrono::steady_clock>;
using HighResolutionTimer = _Timer<std::chrono::high_resolution_clock>;
}