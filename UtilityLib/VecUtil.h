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
int Compare( Iter_1 vec_begin_1, Iter_2 vec_begin_2, size_t n );

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
//{a,b,c}->{0,1,2}
template <typename Iter, typename _Func, typename UIDtype = int>
LFA::unordered_map<UIDtype, size_t> VecIndexing( Iter begin, Iter end, const _Func get_uid )
{
	LFA::unordered_map<UIDtype, size_t> ret;
	ret.reserve( std::distance( begin, end ) );
	for( auto i = begin; i != end; ++i )
	{
		ret[get_uid( *i )];
	}
	size_t idx = 0;
	for( auto& e : ret )
		e.second = idx++;
	return ret;
}
template <typename Iter, typename UIDtype = int>
LFA::unordered_map<UIDtype, size_t> VecIndexing( Iter begin, Iter end )
{
	return VecIndexing( begin, end, [] ( auto v )->UIDtype
	{
		return v;
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
	for( int i = 0; i < (int)n; i++ )
	{
		int idx = *idx_begin++;
		*result_begin++ = *( vec_begin + idx );
	}
}

//Merge same element into <elm,count>
template <typename T, typename Iter>
inline std::vector<std::pair<T, int>> VecCount2pair( Iter begin, Iter end )
{
	std::vector<std::pair<T, int>> ret;
	ret.reserve( std::distance( begin, end ) );
	for( auto i = begin; i != end; ++i )
		if( ret.empty() || *i != ret.back().first )
			ret.emplace_back( *i, 1 );
		else
			++ret.back().second;
	return ret;
}
//Merge same element into count list
template <typename Iter, typename Comparator = std::equal_to<>>
inline std::vector<int> VecCount( Iter begin, Iter end, const Comparator is_eq = std::equal_to<>() )
{
	std::vector<int> ret;
	ret.reserve( std::distance( begin, end ) );
	auto before = begin;
	for( auto i = begin; i != end; ++i )
		if( ret.empty() || !is_eq( *i, *before ) )
		{
			ret.emplace_back( 1 );
			before = i;
		}
		else
			++ret.back();
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

inline std::vector<int> GenerateVec( int n, int st = 0, int step = 1 )
{
	std::vector<int> ret;
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

//return LT(-1) EQ(0) GT(1)
template<typename Iter_1, typename Iter_2>
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