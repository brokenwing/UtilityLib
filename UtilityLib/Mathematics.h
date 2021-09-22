#pragma once
#include "pch.h"
#include "CommonDef.h"

namespace Util::Math
{
typedef std::int64_t LL;

extern inline LL modulo( LL base, LL exponent, const LL mod );//Fast Calc of base^exponent % mod
extern inline LL mulmod( LL a, LL b, const LL mod );//calculates (a * b) % mod taking into account that a * b might overflow
bool MillerRabin( const LL p, int numOftest, RNG& rng );//large prime check
bool isPrime( const LL p );//exact prime check
LL GCD( LL a, LL b );
std::pair<LL, LL> ExtGCD( LL a, LL b );

template <typename Iter>
void Normalize( Iter begin, Iter end, int norm = 1 );
template <typename Iter>
inline double Mean( Iter begin, Iter end )
{
	auto n = std::distance( begin, end );
	return std::accumulate<Iter, double>( begin, end, 0.0 ) / n;
}
template <typename Iter>
double Variance( Iter begin, Iter end, double mean )
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
T FastExponentiation( const T& base, LL exponent, Multiply mul );//General Fast Exponentiation

//
//Body
//
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
}