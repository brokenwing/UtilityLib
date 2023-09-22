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

LL LCM( LL a, LL b )
{
	return a / GCD( a, b ) * b;
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
LL Inverse( const LL a, const LL n )
{
	auto [x, y] = ExtGCD( a, n );
	if( x < 0 )
		x += n;
	return x;
}

//n!
LL Factorial( int n )
{
	LL ret = 1;
	while( n > 0 )
		ret *= n--;
	return ret;
}

//P(n,m):= n*(n-1)*...*(n-m+1)
LL Permutation( int n, int m )
{
	LL ret = 1;
	for( int i = std::max( 2, n - m + 1 ); i <= n; i++ )
		ret *= i;
	return ret;
}

//C(n,m):= P(n,m)/m!
LL Combination( int n, int m )
{
	return Permutation( n, m ) / Factorial( m );
}
}