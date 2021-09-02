#include "pch.h"
#include "Timer.h"

using namespace Util;

#ifdef _WIN32
HighResolutionTimer::HighResolutionTimer()
{
	LARGE_INTEGER nFreq;
	const auto r = QueryPerformanceFrequency( &nFreq );
	if( r )
	{
		m_clocks_cycle = 1.0 / nFreq.QuadPart;
		SetTime();
	}
}

void HighResolutionTimer::SetTime()
{
	if( m_clocks_cycle )//m_clocks_cycle != 0
	{
		const auto r = QueryPerformanceCounter( &t );
		assert( r );
	}
	m_backup.SetTime();
}

std::int64_t Util::HighResolutionTimer::GetMicroseconds() const
{
	return static_cast<std::int64_t>( GetTime() * 1000000 );
}

double HighResolutionTimer::GetTime() const
{
	if( m_clocks_cycle )//m_clocks_cycle != 0
	{
		LARGE_INTEGER curr;
		const auto r = QueryPerformanceCounter( &curr );
		if( r )
			return static_cast<double>( curr.QuadPart - t.QuadPart ) * m_clocks_cycle;
	}
	return m_backup.GetTime();
}
#endif

Util::Timer::Timer() noexcept
{
	SetTime();
}

void Util::Timer::SetTime() noexcept
{
	t = clock();
}

std::int64_t Util::Timer::GetMilliseconds() const noexcept
{
	return static_cast<std::int64_t>( clock() - t ) * 1000 / CLOCKS_PER_SEC;
}

double Util::Timer::GetTime() const noexcept
{
	return static_cast<double>( clock() - t ) / CLOCKS_PER_SEC;
}
