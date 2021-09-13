#pragma once
#include "pch.h"
#include "CommonDef.h"
#include <iostream>

namespace Util
{
//Float Compare
extern constexpr bool 			IsZero( const double v )		noexcept;
extern constexpr bool			EQ( const double a, const double b )	noexcept;
extern constexpr bool			NEQ( const double a, const double b )	noexcept;
extern constexpr bool			LT( const double a, const double b )	noexcept;
extern constexpr bool			LE( const double a, const double b )	noexcept;
extern constexpr bool			GT( const double a, const double b )	noexcept;
extern constexpr bool			GE( const double a, const double b )	noexcept;

//Float Computation
extern inline double		Floor( const double num );//with eps
extern inline double		Ceil( const double num );//with eps
extern constexpr int		Floor( int val, int base );//ex. Floor(120,100)=100, Floor(-120,100)=-200
extern constexpr int		Ceil( int val, int base );//ex. Ceil(120,100)=200, Ceil(-120,100)=-100

//Bit Computation
template <typename T>
inline constexpr T shl( const T v, std::uint8_t bit )noexcept;//with cycle
template <typename T>
inline constexpr T shr( const T v, std::uint8_t bit )noexcept;//with cycle

//Data Move
template <typename T>
inline constexpr void XORswap( T& x, T& y )noexcept;
template<typename _FwdIt>
void Move( _FwdIt begin, _FwdIt const end );
template<typename _FwdIt>
void Move( _FwdIt begin, _FwdIt const end, const int n );

//ToString
template <class ... TParams>
LFA::wstring ToWString( const TParams &... params );
extern inline LFA::wstring _ToWStringImpl_( const LFA::wstringstream& ss );
template <class THead, class ... TTail>
LFA::wstring _ToWStringImpl_( LFA::wstringstream& ss, const THead& head, const TTail& ... tail );

template <class ... TParams>
LFA::string ToString( const TParams &... params );
extern inline LFA::string _ToStringImpl_( const LFA::stringstream& ss );
template <class THead, class ... TTail>
LFA::string _ToStringImpl_( LFA::stringstream& ss, const THead& head, const TTail& ... tail );

//DEBUG
void OpenConsole( bool read = true, bool write = true );
void CloseConsole();
template <typename T>
inline void Println( T&& v );
template<class T, class... Args>
inline void Println( T&& head, Args&&... args );
template <typename Iter>
void PrintVec( Iter begin, Iter end, char delimiter = ' ', char last = '\n' );

//
//	Body
//
template <typename T>
constexpr T shl( const T v, std::uint8_t bit )noexcept
{
	bit %= sizeof( T ) << 3;
	return ( v << bit ) | ( v >> ( ( sizeof( T ) << 3 ) - bit ) );
}

template <typename T>
constexpr T shr( const T v, std::uint8_t bit )noexcept
{
	bit %= sizeof( T ) << 3;
	return ( v >> bit ) | ( v << ( ( sizeof( T ) << 3 ) - bit ) );
}

template <typename T>
constexpr void XORswap( T& x, T& y )noexcept
{
	x ^= y;
	y ^= x;
	x ^= y;
}

template<typename _FwdIt>
void Move( _FwdIt begin, _FwdIt const end )
{
	auto val = *begin;
	auto next = ( begin < end ) ? 1 : -1;
	while( begin != end )
	{
		*begin = *( begin + next );
		begin += next;
	}
	*begin = val;
}

template<typename _FwdIt>
void Move( _FwdIt begin, _FwdIt const end, const int n )
{
	thread_local std::vector<std::remove_reference_t<decltype( *begin )>> cache;
	cache.reserve( n );
	auto it = begin;
	for( int i = 0; i < n; i++, ++it )
		cache.emplace_back( *it );
	if( begin < end )
	{
		for( it = begin + n; it < end; ++it )
		{
			*begin = *it;
			++begin;
		}
		for( int i = 0; i < n; i++ )
		{
			*begin = cache[i];
			++begin;
		}
	}
	else
	{
		auto pos = begin + ( n - 1 );
		for( it = begin - 1; it >= end; --it )
		{
			*pos = *it;
			--pos;
		}
		for( int i = n - 1; i >= 0; i-- )
		{
			*pos = cache[i];
			--pos;
		}
	}
	cache.clear();
}

template <class ... TParams>
LFA::wstring ToWString( const TParams &... params )
{
	LFA::wstringstream ss;
	return _ToWStringImpl_( ss, params ... );
}

template <class THead, class ... TTail>
LFA::wstring _ToWStringImpl_( LFA::wstringstream& ss, const  THead& head, const  TTail& ... tail )
{
	ss << head;
	return _ToWStringImpl_( ss, tail ... );
}

template<class ...TParams>
LFA::string ToString( const TParams & ...params )
{
	LFA::stringstream ss;
	return _ToStringImpl_( ss, params ... );
}

template<class THead, class ...TTail>
LFA::string _ToStringImpl_( LFA::stringstream& ss, const THead& head, const TTail & ...tail )
{
	ss << head;
	return _ToStringImpl_( ss, tail ... );
}

template <typename T>
inline void Println( T&& v )
{
	std::wcout << v << std::endl;
}

template<class T, class... Args>
inline void Println( T&& head, Args&&... args )
{
	std::wcout << head << ' ';
	Println( args... );
}

template <typename Iter>
void PrintVec( Iter begin, Iter end, char delimiter, char last )
{
	auto i = begin;
	std::wcout << *i;
	++i;
	for( ; i != end; ++i )
		std::wcout << delimiter << *i;
	std::wcout << last;
}
}