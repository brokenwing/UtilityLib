#include "pch.h"
#include "Mathematics.h"
#include <limits>


namespace Util::Math
{
LL modulo( LL base, LL exponent, const LL mod )
{
	assert( mod );
	assert( exponent >= 0 );
	assert( mod <= INT_MAX );
	LL x = 1;
	LL y = base % mod;
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

bool isPrime( const LL p )
{
	if( p < 2 )
		return false;
	int n = (int)std::min( p - 1, (LL)std::ceil( sqrt( p ) ) );
	for( int i = 2; i <= n; i++ )
		if( p % i == 0 )
			return false;
	return true;
}

LL GCD(LL a,LL b)
{
	if( b == 0 )
		return a;
	return GCD( b, a % b );
}

std::pair<LL, LL> ExtGCD( LL a, LL b )
{
	if( b == 0 )
	{
		return std::make_pair( 1, 0 );
	}
	else
	{
		auto r = ExtGCD( b, a % b );
		return std::make_pair( r.second, r.first - a / b * r.second );
	}
}
}