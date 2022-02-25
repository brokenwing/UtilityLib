#pragma once
#include "pch.h"

/*
* Ignore any undefinded type
* Support
* 1. simple type (ex. int,double,...)
* 2. string/wstring
* 3. std::array
* 4. static lengh 1D-array (ex. char[10], int[10])
* 5. serialize/unserialize overload
* 6. vector
* 7. pair
* 8. tuple
* 9. simple struct -AddSimpleEntitySerialization
*		ex. only simple type or simple strcut, no pointer, ok to copy whole struct
* 10. complex struct -AddComplexEntitySerialization(variable name,...)
*		ex. has vector, only needs part of variables,
* 11. recursive
*/

#define AddSimpleEntitySerialization public:\
using serialization_type = simple_entity;

#define AddComplexEntitySerialization(...) public:\
using serialization_type = complex_entity;\
auto make_tuple()const	{	return std::forward_as_tuple(__VA_ARGS__);	}\
auto make_tuple()	{	return std::forward_as_tuple(__VA_ARGS__);	}\

namespace Util::Serialization
{
template <typename T>
concept basic_type = std::same_as<T, char> || std::same_as<T, short> || std::same_as<T, int> || std::same_as<T, long> || std::same_as<T, long long>
|| std::same_as<T, unsigned char> || std::same_as<T, unsigned short> || std::same_as<T, unsigned int> || std::same_as<T, unsigned long> || std::same_as<T, unsigned long long>
|| std::same_as<T, double> || std::same_as<T, float>
|| std::same_as<T, bool> || std::same_as<T, wchar_t>;

template <typename T>
concept string_type = std::is_base_of_v<std::string, T> || std::is_base_of_v<std::wstring, T>;

template <typename T>
concept std_array_type = std::is_base_of_v<std::array<typename T::value_type, sizeof( T ) / sizeof( typename T::value_type )>, T>;
template <typename T>
concept bounded_array_type = std::is_bounded_array_v<T> && std::rank_v<T> == 1 && std::extent_v<T> > 0;

template <std_array_type T>
using std_array_elm_type = typename T::value_type;
template <bounded_array_type T>
using bounded_array_elm_type = std::remove_extent_t<T>;

struct simple_entity{};
struct complex_entity{};

template <typename T>
concept simple_entity_type = std::same_as<typename T::serialization_type, simple_entity>;
template <typename T>
concept complex_entity_type = std::same_as<typename T::serialization_type, complex_entity>;



template <typename _istream, typename _ostream>
requires std::is_base_of_v<std::ios_base, _istream>&& std::is_base_of_v<std::ios_base, _ostream>
class basic_serialization
{
public:
	using istream_type = _istream;
	using ostream_type = _ostream;

	virtual int serialize( ostream_type& out )const = 0;
	virtual int unserialize( istream_type& out ) = 0;
};

class serialization :public basic_serialization<std::istream, std::ostream>
{
public:
	//recursive

	template <typename T>
	static void serialize_recursively( const T &val, ostream_type& out )
	{
		if constexpr( std::is_base_of_v<serialization, T> )
			val.serialize( out );
		else if constexpr( basic_type<T> )
			serialize_basic_type( val, out );
		else if constexpr( string_type<T> )
			serialize_string( val, out );
		else if constexpr( std::_Is_specialization_v<T, std::pair> )
		{
			serialize_recursively( val.first, out );
			serialize_recursively( val.second, out );
		}
		else if constexpr( std::_Is_specialization_v<T, std::tuple> )
		{
			if constexpr( !std::same_as<T, std::tuple<>> )
			{
				serialize_recursively( std::get<0>( val ), out );
				serialize_recursively( val._Get_rest(), out );
			}
		}
		else if constexpr( std_array_type<T> )
		{
			if constexpr( basic_type<std_array_elm_type<T>> || simple_entity_type<std_array_elm_type<T>> )
				serialize_entity( val, out );
			else
				for( auto& e : val )
					serialize_recursively( e, out );
		}
		else if constexpr( bounded_array_type<T> )
		{
			if constexpr( basic_type<bounded_array_elm_type<T>> || simple_entity_type<bounded_array_elm_type<T>> )
				serialize_entity( val, out );
			else
				for( auto& e : val )
					serialize_recursively( e, out );
		}
		else if constexpr( std::_Is_specialization_v<T, std::vector> )
		{
			serialize_basic_type<size_t>( val.size(), out );
			for( auto& e : val )
				serialize_recursively( e, out );
		}
		else if constexpr( simple_entity_type<T> )
			serialize_entity( val, out );
		else if constexpr( complex_entity_type<T> )
			serialize_recursively( val.make_tuple(), out );
	}

	template <typename T>
	static void unserialize_recursively( T& val, istream_type& in )
	{
		if constexpr( std::is_base_of_v<serialization, T> )
			val.unserialize( in );
		else if constexpr( basic_type<T> )
			unserialize_basic_type( val, in );
		else if constexpr( string_type<T> )
			unserialize_string( val, in );
		else if constexpr( std::_Is_specialization_v<T, std::pair> )
		{
			unserialize_recursively( val.first, in );
			unserialize_recursively( val.second, in );
		}
		else if constexpr( std::_Is_specialization_v<T, std::tuple> )
		{
			if constexpr( !std::same_as<T, std::tuple<>> )
			{
				unserialize_recursively( std::get<0>( val ), in );
				unserialize_recursively( val._Get_rest(), in );
			}
		}
		else if constexpr( std_array_type<T> )
		{
			if constexpr( basic_type<std_array_elm_type<T>> || simple_entity_type<std_array_elm_type<T>> )
				unserialize_entity( val, in );
			else
				for( auto& e : val )
					unserialize_recursively( e, in );
		}
		else if constexpr( bounded_array_type<T> )
		{
			if constexpr( basic_type<bounded_array_elm_type<T>> || simple_entity_type<bounded_array_elm_type<T>> )
				unserialize_entity( val, in );
			else
				for( auto& e : val )
					unserialize_recursively( e, in );
		}
		else if constexpr( std::_Is_specialization_v<T, std::vector> )
		{
			size_t n = 0;
			unserialize_basic_type<size_t>( n, in );
			val.clear();
			val.resize( n );
			for( auto& e : val )
				unserialize_recursively( e, in );
		}
		else if constexpr( simple_entity_type<T> )
			unserialize_entity( val, in );
		else if constexpr( complex_entity_type<T> )
		{
			auto tmp = val.make_tuple();
			unserialize_recursively( tmp, in );
		}
	}

	//basic type

	template <basic_type T>
	static void serialize_basic_type( const T val, ostream_type& out )
	{
		out.write( reinterpret_cast<const char*>( &val ), sizeof( T ) );
	}
	template <basic_type T>
	static void unserialize_basic_type( T& val, istream_type& in )
	{
		in.read( reinterpret_cast<char*>( &val ), sizeof( T ) );
	}

	//struct

	template <typename T>
	static void serialize_entity( const T &val, ostream_type& out )
	{
		out.write( reinterpret_cast<const char*>( &val ), sizeof( T ) );
	}
	template <typename T>
	static void unserialize_entity( T& val, istream_type& in )
	{
		in.read( reinterpret_cast<char*>( &val ), sizeof( T ) );
	}

	//string, wstring

	static void serialize_string( const std::string& s, ostream_type& out )
	{
		const size_t n = s.size();
		serialize_basic_type<size_t>( n, out );
		out.write( s.data(), n );
	}
	static void serialize_string( const std::wstring& s, ostream_type& out )
	{
		const size_t n = s.size() * sizeof( std::wstring::value_type );
		serialize_basic_type<size_t>( n, out );
		out.write( reinterpret_cast<const char*>( s.data() ), n );
	}

	static void unserialize_string( std::string& s, istream_type& in )
	{
		s.clear();
		size_t si = 0;
		unserialize_basic_type( si, in );
		s.resize( si );
		in.read( s.data(), si );
	}
	static void unserialize_string( std::wstring& s, istream_type& in )
	{
		s.clear();
		size_t si = 0;
		unserialize_basic_type( si, in );
		s.resize( si / sizeof( std::wstring::value_type ) );
		in.read( reinterpret_cast<char*>( s.data() ), si );
	}
};
}