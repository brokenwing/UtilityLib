#pragma once
#include "pch.h"

//vector computation
namespace Util
{
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
}