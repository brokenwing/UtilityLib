#pragma once
#include "pch.h"
#include "CommonDef.h"

namespace Util
{
//Vector Compare
template <typename Iter_1, typename Iter_2>
bool EQ( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n );
template <typename Iter_1, typename Iter_2>
bool NEQ( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n );
template <typename Iter_1, typename Iter_2>
bool LT( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n );
template <typename Iter_1, typename Iter_2>
bool GT( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n );
template <typename Iter_1, typename Iter_2>
bool LE( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n );
template <typename Iter_1, typename Iter_2>
bool GE( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n );
//return LT(-1) EQ(0) GT(1)
template <typename Iter_1, typename Iter_2>
int Compare( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n )
{
	while( n-- )
	{
		if( *vec_begin_1 != *vec_begin_2 )
			return ( *vec_begin_1 < *vec_begin_2 ) ? -1 : 1;
		++vec_begin_1;
		++vec_begin_2;
	}
	return 0;
}

template <typename Iter, typename Cmp>
bool isVectorOrdered( Iter begin, Iter end, Cmp cmp );
template <typename Iter>
bool isAscend( Iter begin, Iter end )
{
	return isVectorOrdered( begin, end, std::less<>() );
}
template <typename Iter>
bool isAscendOrEqual( Iter begin, Iter end )
{
	return isVectorOrdered( begin, end, std::less_equal<>() );
}
template <typename Iter>
bool isDescend( Iter begin, Iter end, bool is_equal = false )
{
	return isVectorOrdered( begin, end, std::greater<>() );
}
template <typename Iter>
bool isDescendOrEqual( Iter begin, Iter end, bool is_equal = false )
{
	return isVectorOrdered( begin, end, std::greater_equal<>() );
}
template <typename Iter>
bool isSame( Iter begin, Iter end )
{
	return isVectorOrdered( begin, end, [] ( const auto& l, const auto& r )->bool
	{
		return l == r;
	} );
}

//Vector

template <typename Iter, typename T>
void VecAdd( Iter begin, Iter end, const T& val )
{
	for( auto i = begin; i != end; ++i )
		*i += val;
}
template <typename Iter, typename T>
void VecDec( Iter begin, Iter end, const T& val )
{
	for( auto i = begin; i != end; ++i )
		*i -= val;
}
template <typename Iter, typename T>
void VecMul( Iter begin, Iter end, const T& val )
{
	for( auto i = begin; i != end; ++i )
		*i *= val;
}
template <typename Iter, typename T>
void VecDiv( Iter begin, Iter end, const T& val )
{
	for( auto i = begin; i != end; ++i )
		*i /= val;
}
template <typename Iter, typename T>
void VecMod( Iter begin, Iter end, const T& val )
{
	for( auto i = begin; i != end; ++i )
		*i %= val;
}
//index start from 0
template <typename Iter_1,typename Iter_2>
inline void SubVec( Iter_1 vec_begin, Iter_2 idx_begin, Iter_1 result_begin, size_t n)
{
	static_assert( std::_Is_random_iter_v<Iter_1> || std::is_array_v<Iter_1> );
	for( int i = 0; i < n; i++ )
	{
		int idx = *idx_begin++;
		*result_begin++ = *( vec_begin + idx );
	}
}

//Merge same element into <elm,count>
template <typename T, typename Iter>
inline LFA::vector<std::pair<T, int>> VecCount( Iter begin, Iter end )
{
	LFA::vector<std::pair<T, int>> ret;
	for( auto i = begin; i != end; ++i )
		if( ret.empty() || *i != ret.back().first )
			ret.emplace_back( *i, 1 );
		else
			++ret.back().second;
	return ret;
}

template <typename T, typename Iter>
inline size_t VecHash( Iter begin, Iter end )
{
	size_t val = 0;
	std::hash<T> h;
	for( auto i = begin; i != end; ++i )
	{
		size_t nxt = h( *i );
		val = val * nxt + nxt;
	}
	return val;
}

inline LFA::vector<int> GenerateVec( int n, int st = 0, int step = 1 )
{
	LFA::vector<int> ret;
	ret.reserve( n );
	FOR( i, 0, n )
		ret.emplace_back( st + i * step );
	return ret;
}

//
//body
//

template <typename Iter_1, typename Iter_2>
bool EQ( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n )
{
	while( n-- )
	{
		if( *vec_begin_1++ != *vec_begin_2++ )
			return false;
	}
	return true;
}
template <typename Iter_1, typename Iter_2>
bool NEQ( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n )
{
	return !EQ( vec_begin_1, vec_begin_2, n );
}
template <typename Iter_1, typename Iter_2>
bool LT( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n )
{
	while( n-- )
	{
		if( *vec_begin_1 < *vec_begin_2 )
			return true;
		else if( *vec_begin_1 > *vec_begin_2 )
			return false;
		++vec_begin_1;
		++vec_begin_2;
	}
	return false;//EQ
}
template <typename Iter_1, typename Iter_2>
bool GT( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n )
{
	while( n-- )
	{
		if( *vec_begin_1 > *vec_begin_2 )
			return true;
		else if( *vec_begin_1 < *vec_begin_2 )
			return false;
		++vec_begin_1;
		++vec_begin_2;
	}
	return false;//EQ
}
template <typename Iter_1, typename Iter_2>
bool LE( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n )
{
	return !GT( vec_begin_1, vec_begin_2, n );
}
template <typename Iter_1, typename Iter_2>
bool GE( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n )
{
	return !LT( vec_begin_1, vec_begin_2, n );
}

template <typename Iter, typename Cmp>
bool isVectorOrdered( Iter begin, Iter end, Cmp cmp )
{
	if( begin == end )
		return true;
	auto x = begin;
	auto y = begin + 1;
	for( ; y != end; ++x, ++y )
	{
		if( !cmp( *x, *y ) )
			return false;
	}
	return true;
}
}