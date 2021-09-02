#include "pch.h"
#include "Math.h"

namespace Util::Math
{
LL modulo( LL base, LL exponent, const LL mod )
{
	assert( mod );
	LL x = 1;
	LL y = base;
	while( exponent > 0 )
	{
		if( ( exponent & 1 ) == 1 )
			x = ( x * y ) % mod;
		y = ( y * y ) % mod;
		exponent >>= 1;
	}
	return x % mod;
}

LL mulmod( LL a, LL b, const LL mod )
{
	assert( mod );
	LL x = 0, y = a % mod;
	while( b > 0 )
	{
		if( ( b & 1 ) == 1 )
			x = ( x + y ) % mod;
		y = ( y << 1 ) % mod;
		b >>= 1;
	}
	return x % mod;
}
}