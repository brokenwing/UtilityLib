#pragma once
#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

//Compile time indexing tool
namespace Util::Indexing
{
//define necessary get method
struct ElementInterface
{
	template <typename T>
	T GetKey( const int id )const
	{
		return T();
	}
	template <typename T>
	T GetValue( const int id )const
	{
		return T();
	}
};
template <typename T>
concept element_type = std::is_base_of_v<ElementInterface, T>;

//define necessary method for user-indexing class
template <element_type T>
struct BaseIndexing
{
	using value_type = T;

	std::string name;
	int id = -1;

	//constexpr void insert( const value_type& );
	//constexpr void erase( const value_type& );
	//constexpr bool contain( const value_type& )const;
	//constexpr void clear();
	//iterator find( const value_type& elm )
	//const_iterator find( const value_type& elm )const
};
template <typename T>
concept indexing_type = std::is_base_of_v<BaseIndexing<typename T::value_type>, T>;

//linear indexing
template <indexing_type Base, typename T = int>
struct ArrayIndexing :public Base
{
	using value_type = Base::value_type;
	using data_type = T;
	using iterator = std::vector<T>::iterator;
	using const_iterator = std::vector<T>::const_iterator;

	std::vector<T> data;
	std::vector<bool> slot;

	const_iterator find( const value_type& elm )const
	{
		const int key = elm.GetKey<int>( Base::id );
		assert( key >= 0 );
		if( key < static_cast<int>( slot.size() ) && slot[key] )
			return data.begin() + key;
		return data.end();
	}
	iterator find( const value_type& elm )
	{
		const int key = elm.GetKey<int>( Base::id );
		assert( key >= 0 );
		if( key < static_cast<int>( slot.size() ) && slot[key] )
			return data.begin() + key;
		return data.end();
	}
	void insert( const value_type& elm )
	{
		const int key = elm.GetKey<int>( Base::id );
		assert( key >= 0 );
		resize( key + 1 );
		slot[key] = true;
		data[key] = elm.GetValue<data_type>( Base::id );
	}
	void erase( const value_type& elm )
	{
		const int key = elm.GetKey<int>( Base::id );
		assert( key >= 0 );
		if( key < static_cast<int>( slot.size() ) )
			slot[key] = false;
	}
	bool contain( const value_type& elm )const
	{
		const int key = elm.GetKey<int>( Base::id );
		assert( key >= 0 );
		return key < static_cast<int>( slot.size() ) && slot[key];
	}
	void clear()
	{
		data.clear();
		slot.clear();
	}

	const data_type& at( int idx )const
	{
		return data[idx];
	}
	void resize( int n )
	{
		if( n > static_cast<int>( data.size() ) ) [[unlikely]]
		{
			data.resize( n );
			slot.resize( n, false );
		}
	}
};

template <indexing_type Base, typename Key = int, typename Val = int>
struct OrderedIndexing :public Base
{
	using value_type = Base::value_type;
	using key_type = Key;
	using data_type = Val;
	using iterator = std::map<key_type, data_type>::iterator;
	using const_iterator = std::map<key_type, data_type>::const_iterator;

	std::map<key_type, data_type> data;

	const_iterator find( const value_type& elm )const
	{
		return data.find( elm.GetKey<key_type>( Base::id ) );
	}
	iterator find( const value_type& elm )
	{
		return data.find( elm.GetKey<key_type>( Base::id ) );
	}
	void insert( const value_type& elm )
	{
		data.insert_or_assign( elm.GetKey<key_type>( Base::id ), elm.GetValue<data_type>( Base::id ) );
	}
	void erase( const value_type& elm )
	{
		data.erase( elm.GetKey<key_type>( Base::id ) );
	}
	bool contain( const value_type& elm )const
	{
		return data.contains( elm.GetKey<key_type>( Base::id ) );
	}
	void clear()
	{
		data.clear();
	}

	const data_type& first()const
	{
		return data.begin()->second;
	}
};

template <indexing_type Base, typename Key = int, typename Val = int>
struct HashIndexing :public Base
{
	using value_type = Base::value_type;
	using key_type = Key;
	using data_type = Val;
	using iterator = std::unordered_map<key_type, data_type>::iterator;
	using const_iterator = std::unordered_map<key_type, data_type>::const_iterator;

	std::unordered_map<key_type, data_type> data;

	const_iterator find( const value_type& elm )const
	{
		return data.find( elm.GetKey<key_type>( Base::id ) );
	}
	iterator find( const value_type& elm )
	{
		return data.find( elm.GetKey<key_type>( Base::id ) );
	}
	void insert( const value_type& elm )
	{
		data[elm.GetKey<key_type>( Base::id )] = elm.GetValue<data_type>( Base::id );
	}
	void erase( const value_type& elm )
	{
		data.erase( elm.GetKey<key_type>( Base::id ) );
	}
	bool contain( const value_type& elm )const
	{
		return data.contains( elm.GetKey<key_type>( Base::id ) );
	}
	void clear()
	{
		data.clear();
	}
};



//manage different type of index of same data
template <indexing_type ...Args>
class MultipleIndexing :public std::tuple<Args...>
{
public:
	using tuple_type = std::tuple<Args...>;

	MultipleIndexing() :tuple_type()
	{
		//reset id from 0
		int i = 0;
		for_each( [&i] ( auto& e )->void
		{
			e.id = i++;
		} );
	}
	constexpr std::size_t size()const
	{
		return std::tuple_size_v<tuple_type>;
	}
	template <int idx>
	constexpr auto& at()
	{
		return std::get<idx>( *this );
	}
	template <typename Func>
	void for_each( Func fn )
	{
		_for_each( fn );
	}
	void clear()
	{
		for_each( [] ( auto& e )->void
		{
			e.clear();
		} );
	}
	template <element_type T>
	void insert( const T& elm )
	{
		for_each( [&elm] ( auto& e )->void
		{
			e.insert( elm );
		} );
	}
	template <element_type T>
	void erase( const T& elm )
	{
		for_each( [&elm] ( auto& e )->void
		{
			e.erase( elm );
		} );
	}

private:
	template <typename Func, size_t Index = 0>
	constexpr void _for_each( Func fn )
	{
		if constexpr( Index < std::tuple_size_v<tuple_type> )
		{
			fn( std::get<Index>( *this ) );
			_for_each<Func, Index + 1>( fn );
		}
	}
};
}