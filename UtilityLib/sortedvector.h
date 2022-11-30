#pragma once
#include "pch.h"
#include "Traits.h"
#include <vector>

//for small data set
namespace Util
{
template<typename _Traits>
class _SortedVector :private std::vector<typename _Traits::value_type, typename _Traits::allocator_type>
{
public:
	using key_type = typename _Traits::key_type;
	using value_type = typename _Traits::value_type;
	using key_compare_type = typename _Traits::key_compare_type;
	using value_compare_type = typename _Traits::value_compare_type;
	using key_equal_type = typename _Traits::key_equal_type;
	using allocator_type = typename _Traits::allocator_type;
	using container = std::vector<value_type, allocator_type>;
	using iterator = std::conditional_t<std::same_as<key_type, value_type>, typename container::const_iterator, typename container::iterator>;//set->const iterator; map->iterator
	using const_iterator = typename container::const_iterator;

public:
	using container::container;
	using container::reserve;
	using container::size;
	using container::empty;
	using container::clear;
	using container::erase;
	
	const_iterator begin()const
	{
		return container::cbegin();
	}
	iterator begin()
	{
		return container::begin();
	}
	const_iterator end()const
	{
		return container::cend();
	}
	iterator end()
	{
		return container::end();
	}
	//1-n
	const value_type& find_by_rank( int pos )const
	{
		return container::operator[]( pos - 1 );
	}
	const container& toVector()const noexcept
	{
		return *this;
	}
	bool check()const
	{
		if( size() <= 1 )
			return true;
		auto x = begin();
		auto y = x;
		++y;
		for( ; y != end(); ++x, ++y )
		{
			if constexpr( _Traits::MultiSet )
			{
				if( value_compare_type()( *y, *x ) )
					return false;
			}
			else
			{
				if( !value_compare_type()( *x, *y ) )
					return false;
			}
		}
		return true;
	}
	void erase( const key_type& key )
	{
		if( auto r = find( key ); r != end() )
			erase( r );
	}
	//assume small size, no binary search
	const_iterator find( const key_type& key )const
	{
		for( auto i = begin(); i != end(); ++i )
			if( key_equal_type()( _Traits::get_key( *i ), key ) )
				return i;
		return end();
	}
	//assume small size, no binary search
	iterator find( const key_type& key )
	{
		for( auto i = begin(); i != end(); ++i )
			if( key_equal_type()( _Traits::get_key( *i ), key ) )
				return i;
		return end();
	}
	iterator insert( const value_type& val )
	{
		assert( check() );
		for( auto i = begin(); i != end(); ++i )
			if( value_compare_type()( val, *i ) )
			{
				return container::emplace( i, val );
			}
			else
			{
				if constexpr( !_Traits::MultiSet )
					if( !value_compare_type()( *i, val ) )
						return i;
			}
		container::emplace_back( val );
		return end() - 1;
	}
};

template <typename T, typename _Comp = std::less<T>, typename _Alloc = std::allocator<T>>
class sortedvectorSet :public _SortedVector<_Tset_traits<T, _Comp, _Alloc, false>>
{};

template <typename T, typename _Comp = std::less<T>, typename _Alloc = std::allocator<T>>
class sortedvectorMultiSet :public _SortedVector<_Tset_traits<T, _Comp, _Alloc, true>>
{};

template <typename _Key, typename _Value, typename _Comp = std::less<_Key>, typename _Alloc = std::allocator<std::pair<_Key, _Value>>>
class sortedvectorMap :public _SortedVector<_Tmap_traits<_Key, _Value, _Comp, _Alloc, false>>
{
private:
	using _Mybase = _SortedVector<_Tmap_traits<_Key, _Value, _Comp, _Alloc, false>>;

public:
	_Value& operator[]( const _Key& key )
	{
		return _Mybase::insert( typename _Mybase::value_type( key, _Value() ) )->value();
	}
};
}