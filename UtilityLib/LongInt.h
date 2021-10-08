#pragma once
#include "RawLongInt.h"

namespace Util
{
//RADIX = 10^8
class LongInt :public RawLongInt<100000000>
{
public:
	LongInt() :RawLongInt()
	{}
	LongInt( const RawULongInt& val ) :RawLongInt( val )
	{}
	LongInt( const ULongInt& val ) :RawLongInt( val )
	{}
	LongInt( const RawLongInt& val ) :RawLongInt( val )
	{}
	LongInt( int val ) :RawLongInt( val )
	{}
	LongInt( const LFA::string& s, char delimiter = ' ' ) :RawLongInt( s, delimiter )
	{}
	~LongInt()
	{}
	
	LongInt& operator++()
	{
		RawLongInt::operator++();
		return *this;
	}
	LongInt& operator--()
	{
		RawLongInt::operator--();
		return *this;
	}
	LongInt operator++(int)
	{
		return RawLongInt::operator++( 0 );
	}
	LongInt operator--(int)
	{
		return RawLongInt::operator--( 0 );
	}

	LongInt operator+( const LongInt& other )const
	{
		return RawLongInt::operator+( other );
	}
	LongInt operator-( const LongInt& other )const
	{
		return RawLongInt::operator-( other );
	}
	LongInt operator*( const LongInt& other )const
	{
		return RawLongInt::operator*( other );
	}
	LongInt operator/( const LongInt& other )const
	{
		return RawLongInt::operator/( other );
	}
	LongInt operator%( const LongInt& other )const
	{
		return RawLongInt::operator%( other );
	}
	LongInt operator*( const int other )const
	{
		return RawLongInt::operator*( other );
	}
	LongInt operator/( const int other )const
	{
		return RawLongInt::operator/( other );
	}
	LongInt operator%( const int other )const
	{
		return RawLongInt::operator%( other );
	}
	
	LongInt& operator+=( const LongInt& other )
	{
		*this = operator+( other );
		return *this;
	}
	LongInt& operator-=( const LongInt& other )
	{
		*this = operator-( other );
		return *this;
	}
	LongInt& operator*=( const LongInt& other )
	{
		*this = operator*( other );
		return *this;
	}
	LongInt& operator/=( const LongInt& other )
	{
		*this = operator/( other );
		return *this;
	}
	LongInt& operator%=( const LongInt& other )
	{
		*this = operator%( other );
		return *this;
	}

	LongInt operator^( const size_t exponent )const
	{
		return RawLongInt::operator^( exponent );
	}
	LongInt PowerMod( ULongInt exponent, const LongInt& mod )const
	{
		return RawLongInt::PowerMod( exponent, mod );
	}

	LFA::string ToString( char delimiter = 0 ) const
	{
		LFA::string ret;
		ret.reserve( (size_t)n * 8 + 1 );
		if( !sign && !isZero() )
			ret += '-';
		char buf[15];
		for( int i = n - 1; i >= 0; --i )
		{
			sprintf_s( buf, ( i == n - 1 ) ? "%u" : "%08u", m_val[i] );
			ret += buf;
			if( delimiter && i != 0 )
				ret += delimiter;
		}
		return ret;
	}
};
}