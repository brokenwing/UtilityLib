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
template<typename Engine = Util::RNG>
bool MillerRabin( const LL p, int numOftest, Engine& rng );
//exact prime check
bool isPrime( const LL p );
LL GCD( LL a, LL b );
std::pair<LL, LL> ExtGCD( LL a, LL b );
//find x s.t. a*x %n == gcd(a,n)
extern inline LL Inverse( const LL a, const LL n );
//n!
extern inline LL Factorial( int n );
//P(n,m):= n*(n-1)*...*(n-m+1)
extern inline LL Permutation( int n, int m );
//C(n,m):= P(n,m)/m!
extern inline LL Combination( int n, int m );

template<typename T>
T GCD( T a, T b );
template<typename T>
std::pair<T, T> ExtGCD( const T& a, const T& b );
template<typename T>
T Inverse( const T& a, const T& n );
template<typename T>
T Factorial( int n );
template<typename T>
T Permutation( int n, int m );
template<typename T>
T Combination( int n, int m );

template <typename Iter>
void Normalize( Iter begin, Iter end, int norm = 1 );
template <typename Iter>
inline double Mean( Iter begin, Iter end );
template <typename Iter>
inline double Variance( Iter begin, Iter end, double mean );
template <typename Iter>
inline double Variance( Iter begin, Iter end );

template <typename T, typename Multiply>
T FastExponentiation( const T& base, LL exponent, Multiply mul );//General Fast Exponentiation
template <typename T, typename Multiply, typename Modulo>
T FastExponentiation( const T& base, LL exponent, const T& mod, Multiply mul, Modulo modulo );//General Fast Exponentiation with mod


//
//Body
//

template<typename Engine>
bool MillerRabin( const LL p, int numOftest, Engine& rng )
{
	if( p < 2 )
		return false;
	if( p == 2 )
		return true;
	if( ( p & 1 ) == 0 )
		return false;
	std::uniform_int_distribution<LL> rand_LL( 2, p - 1 );
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

template <typename Iter>
void Normalize( Iter begin, Iter end, int norm )
{
	double sum = 0;
	switch( norm )
	{
	case 0:
		for( auto i = begin; i != end; ++i )
			sum += 1;
		break;
	case 1:
		for( auto i = begin; i != end; ++i )
			sum += *i;
		break;
	case 2:
		for( auto i = begin; i != end; ++i )
			sum += ( *i ) * ( *i );
		break;
	default:
		for( auto i = begin; i != end; ++i )
			sum += std::pow( *i, norm );
		break;
	}
	assert( sum > eps );
	for( auto i = begin; i != end; ++i )
		*i /= sum;
}
template <typename Iter>
inline double Mean( Iter begin, Iter end )
{
	auto n = std::distance( begin, end );
	return std::accumulate<Iter, double>( begin, end, 0.0 ) / n;
}
template <typename Iter>
inline double Variance( Iter begin, Iter end, double mean )
{
	double ret = 0;
	for( auto i = begin; i != end; ++i )
	{
		double v = *i - mean;
		ret += v * v;
	}
	return ret;
}
template <typename Iter>
inline double Variance( Iter begin, Iter end )
{
	return Variance( begin, end, Mean( begin, end ) );
}
template <typename T,typename Multiply>
T FastExponentiation( const T& base, LL exponent, Multiply mul )
{
	if( exponent <= 1 )
		return base;
	T x = base;
	T y = base;
	--exponent;
	while( exponent > 0 )
	{
		if( ( exponent & 1 ) == 1 )
			x = mul( x, y );//x*=y
		y = mul( y, y );//y*=y
		exponent >>= 1;
	}
	return x;
}
template <typename T, typename Multiply, typename Modulo>
T FastExponentiation( const T& base, size_t exponent, const T& mod, Multiply mul, Modulo modulo )//General Fast Exponentiation with mod
{
	T x = 1;
	T y = modulo( base, mod );
	while( exponent > 0 )
	{
		if( ( exponent & 1 ) == 1 )
			x = modulo( mul( x, y ), mod );
		y = modulo( mul( y, y ), mod );
		exponent >>= 1;
	}
	return modulo( x, mod );
}
template<typename T>
T GCD( T a, T b )
{
	T tmp;
	while( true )
	{
		if( b == T( 0 ) )
			return a;
		tmp = a;
		a = b;
		b = tmp % b;
	}
}
template<typename T>
std::pair<T, T> ExtGCD( const T& a, const T& b )
{
	if( b == T( 0 ) )
	{
		return std::make_pair( T( 1 ), T( 0 ) );
	}
	else
	{
		auto r = ExtGCD( b, a % b );
		return std::make_pair( r.second, r.first - a / b * r.second );
	}
}
template<typename T>
T Inverse( const T &a, const T &n )
{
	auto [x, y] = ExtGCD<T>( a, n );
	if( x < T(0) )
		x += n;
	return x;
}
template<typename T>
T Factorial( int n )
{
	T ret( 1 );
	while( n > 0 )
		ret *= T( n-- );
	return ret;
}
template<typename T>
T Permutation( int n, int m )
{
	T ret( 1 );
	for( int i = std::max( 2, n - m + 1 ); i <= n; i++ )
		ret *= T( i );
	return ret;
}
template<typename T>
T Combination( int n, int m )
{
	return Permutation<T>( n, m ) / Factorial<T>( m );
}
}