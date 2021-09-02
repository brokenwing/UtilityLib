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

template <typename Iter>
void Normalize( Iter begin, Iter end, int norm = 1 );

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
}