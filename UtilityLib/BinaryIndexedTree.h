#pragma once
#include "CommonDef.h"

namespace Util
{
// Range [0,n)
// Fast Sum of Prefix; modification of single element
//https://en.wikipedia.org/wiki/Fenwick_tree
template <typename ValueType = int>
requires requires( ValueType a, ValueType b )//Addable
{
	a + b;
	a += b;
}
class BinaryIndexedTree
{
private:
	size_t n = 0;
	LFA::vector<ValueType> arr;

public:
	BinaryIndexedTree( size_t n = 0 )
	{
		resize( n );
	}
	~BinaryIndexedTree()
	{}
	void Add( const int pos, const ValueType& val )
	{
		assert( pos >= 0 && pos < n );
		_add_( pos, val );
	}
	//[l,r]
	ValueType SumInterval( const int left, const int right )const
	{
		assert( left < n );
		assert( right < n );
		return _sum_( right ) - _sum_( left - 1 );
	}
	ValueType at( const int index )const
	{
		return SumInterval( index, index );
	}
	ValueType operator[]( const int index )const
	{
		return at( index );
	}
	void clear()
	{
		std::fill( arr.begin(), arr.end(), ValueType() );
	}
	void resize( size_t _n )
	{
		n = _n;
		arr.resize( n + 1, ValueType() );
	}

private:
	constexpr int lowbit( const int x )const
	{
		return x & ( -x );
	}
	//[index,end]
	void _add_( const int index, const ValueType& val )
	{
		for( int j = index + 1; j <= n; j += lowbit( j ) )
			arr[j] += val;
	}
	//[begin,index]
	ValueType _sum_( const int index )const
	{
		ValueType ans = ValueType();
		for( int i = index + 1; i > 0; i -= lowbit( i ) )
			ans += arr[i];
		return ans;
	}
};
}