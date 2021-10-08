#pragma once
#include "RawULongInt.h"

namespace Util
{
//RADIX = 10^8
class ULongInt :public RawULongInt<100000000>
{
public:
	ULongInt() :RawULongInt()
	{}
	ULongInt( unsigned int val ) :RawULongInt( val )
	{}
	ULongInt( const LFA::string& s, char delimiter = ' ' ) :RawULongInt( s, delimiter )
	{}
	~ULongInt()
	{}

	LFA::string ToString( char delimiter = 0 ) const
	{
		LFA::string ret;
		ret.reserve( (size_t)n * 8 );
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