#pragma once
#include "pch.h"

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
}