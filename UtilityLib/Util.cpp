#include "pch.h"
#include "Util.h"
#include "CommonDef.h"
#include <cmath>
#include <Windows.h>

namespace Util
{
constexpr bool IsZero( const double v ) noexcept
{
	return v < eps && v > -eps;
}

constexpr bool EQ( const double a, const double b ) noexcept
{
	return IsZero( a - b );
}

constexpr bool NEQ( const double a, const double b ) noexcept
{
	return !EQ( a, b );
}

constexpr bool LT( const double a, const double b ) noexcept
{
	return eps < b - a;
}

constexpr bool LE( const double a, const double b ) noexcept
{
	return a - b <= eps;
}

constexpr bool GT( const double a, const double b ) noexcept
{
	return a - b > eps;
}

constexpr bool GE( const double a, const double b ) noexcept
{
	return  eps >= b - a;
}

double Floor( const double num )
{
	return std::floor( num + eps );
}

double Ceil( const double num )
{
	return std::ceil( num - eps );
}

//ex. Floor(120,100)=100, Floor(-120,100)=-200
constexpr int Floor( int val, int base )
{
	const int mod = val % base;
	return val - mod - ( mod < 0 ) * base;
}

//ex. Ceil(120,100)=200, Ceil(-120,100)=-100
constexpr int Ceil( int val, int base )
{
	const int mod = val % base;
	return val - mod + ( mod > 0 ) * base;
}

LFA::wstring _ToWStringImpl_( const LFA::wstringstream& ss )
{
	return ss.str();
}

LFA::string _ToStringImpl_( const LFA::stringstream& ss )
{
	return ss.str();
}

void OpenConsole( bool read, bool write )
{
	FILE* stream = nullptr;
	AllocConsole();
	if( read )
	{
		auto r = freopen_s( &stream, "CONIN$", "r", stdin );
		assert( r );
	}
	if( write )
	{
		auto r = freopen_s( &stream, "CONOUT$", "w", stdout );
		assert( r );
	}
}
void CloseConsole()
{
	FreeConsole();
}

int GetLogicalCoreCount()noexcept
{
	try
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo( &sysinfo );
		return sysinfo.dwNumberOfProcessors;
	} catch( ... )
	{
		return 0;
	}
	return 0;
}

int GetPhysicalCoreCount()noexcept
{
	int nProcessors = 0;
	try
	{
		DWORD len = 0;
		GetLogicalProcessorInformation( nullptr, &len );
		std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer;
		buffer.resize( len / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION ) );

		if( GetLogicalProcessorInformation( buffer.data(), &len ) )
		{
			for( const auto& info : buffer )
			{
				if( info.Relationship == _LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore )
				{
					++nProcessors;
				}
			}
		}
	} catch( ... )
	{
		return 0;
	}
	return nProcessors;
}

std::pair<int, int> GetPEcoreCount()noexcept
{
	int nPerformanceCores = 0;
	int nEfficiencyCores = 0;
	try
	{
		DWORD len = 0;
		GetLogicalProcessorInformation( nullptr, &len );
		std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer( len / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION ) );

		if( GetLogicalProcessorInformation( buffer.data(), &len ) )
		{
			for( const auto& info : buffer )
			{
				if( info.Relationship == RelationProcessorCore )
				{
					if( info.ProcessorCore.Flags & 0x1 )
						++nPerformanceCores;
					else
						++nEfficiencyCores;
				}
			}
		}
	} catch( ... )
	{
		return { 0,0 };
	}
	return { nPerformanceCores,nEfficiencyCores };
}

std::pair<unsigned long long, unsigned long long> GetPEcoreMask()noexcept
{
	ULONG_PTR performanceCoresMask = 0;
	ULONG_PTR efficiencyCoresMask = 0;
	try
	{
		DWORD len = 0;
		GetLogicalProcessorInformation( nullptr, &len );
		std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer( len / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION ) );

		if( GetLogicalProcessorInformation( buffer.data(), &len ) )
		{
			for( const auto& info : buffer )
			{
				if( info.Relationship == RelationProcessorCore )
				{
					if( info.ProcessorCore.Flags & 0x1 )
						performanceCoresMask |= info.ProcessorMask;
					else
						efficiencyCoresMask |= info.ProcessorMask;
				}
			}
		}
	} catch( ... )
	{
		return { 0,0 };
	}
	return { performanceCoresMask,efficiencyCoresMask };
}

void SetThreadAffinity( unsigned long long mask )noexcept
{
	SetThreadAffinityMask( GetCurrentThread(), mask );
}

}