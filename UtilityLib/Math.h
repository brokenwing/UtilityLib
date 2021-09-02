#pragma once
#include "pch.h"
#include "CommonDef.h"

namespace Util::Math
{
typedef std::int64_t LL;

//Fast Calc of base^exponent % mod
extern inline LL modulo( LL base, LL exponent, const LL mod );

//calculates (a * b) % mod taking into account that a * b might overflow
extern inline LL mulmod( LL a, LL b, const LL mod );

//large prime check
bool MillerRabin( const LL p, int numOftest, RNG& rng )
{
	if( p < 2 )
		return false;
	if( p == 2 )
		return true;
	if( ( p & 1 ) == 0 )
		return false;
	std::uniform_int_distribution<LL> rand_LL( 1, p - 1 );
	LL s = p - 1;
	while( ( s & 1 ) == 0 )
		s >>= 1;
	while( numOftest-- )
	{
//LL base = rand() % (p - 1) + 1;
		LL base = rand_LL( rng );
		LL temp = s;
		LL mod = modulo( base, temp, p );
		while( temp != p - 1 && mod != 1 && mod != p - 1 )
		{
			mod = mulmod( mod, mod, p );
			temp <<= 1;
		}
		if( mod != p - 1 && ( temp & 1 ) == 0 )
			return false;
	}
	return true;
}

}