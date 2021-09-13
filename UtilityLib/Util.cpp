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
}