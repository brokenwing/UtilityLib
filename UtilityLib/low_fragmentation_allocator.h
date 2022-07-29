#pragma once
#include <vector>
#include <assert.h>
#include <array>
#include <algorithm>

namespace LFA
{
class FixMemoryAllocator
{
protected:
	struct PageInfo
	{
		std::byte* ptr = nullptr;
		unsigned int len = 0;
	};
	unsigned int ElementSize = 0;
	unsigned int DefaultPageSize = 0;
	std::vector<PageInfo> pages;
	std::vector<std::byte*> free;

public:
	FixMemoryAllocator( unsigned int ElementSize = 4, unsigned int DefaultPageSize = 4 * 1000 ) :ElementSize( ElementSize ), DefaultPageSize( DefaultPageSize )
	{
		assert( DefaultPageSize >= ElementSize );
		pages.reserve( 4 );
	}
	~FixMemoryAllocator()
	{
		for( auto p : pages )
			delete[] p.ptr;
	}
	void deallocate_untyped( std::byte* p )
	{
		free.emplace_back( p );
	}
	std::byte* allocate_untyped()
	{
		if( free.empty() )
			newpage();

		auto ret = free.back();
		free.pop_back();
		return ret;
	}
	void newpage( unsigned int page_size = 0 )
	{
		if( page_size == 0 )
			page_size = DefaultPageSize;
		assert( page_size > 0 );
		auto& info = pages.emplace_back();
		info.len = page_size;
		info.ptr = new std::byte[page_size];
		free.reserve( free.size() + ( page_size / ElementSize ) + 1 );
		for( int i = page_size / ElementSize * ElementSize - ElementSize; i >= 0; i -= ElementSize )
			free.emplace_back( info.ptr + i );
	}
	void shrink_to_fit()
	{
		std::sort( pages.begin(), pages.end(), [] ( auto& a, auto& b )->bool
		{
			return a.ptr < b.ptr;
		} );
		std::sort( free.begin(), free.end() );
		decltype( pages ) new_pages;
		decltype( free ) new_free;

		new_pages.reserve( pages.size() );
		new_free.reserve( free.size() );
		for( auto& cur : pages )
		{
			auto left = std::lower_bound( free.begin(), free.end(), cur.ptr );
			auto right = std::lower_bound( free.begin(), free.end(), cur.ptr + cur.len );
			if( right - left == cur.len / ElementSize )
			{
				delete[] cur.ptr;
			}
			else
			{
				new_pages.emplace_back( cur );
				new_free.insert( new_free.end(), left, right );
			}
		}
		pages.swap( new_pages );
		free.swap( new_free );
		std::sort( free.begin(), free.end(), std::greater<>() );//allocate in increasing order of memory
	}
};

class FragmentManagement
{
public:
	static constexpr int MAX_SIZE = 32;//32*4 byte
	static FixMemoryAllocator& at( const int idx )
	{
		thread_local bool is_initialized = false;
		thread_local std::array<FixMemoryAllocator, MAX_SIZE> alloList;
		if( !is_initialized ) [[unlikely]]
		{
			is_initialized = true;
			for( int i = 0; i < MAX_SIZE; i++ )
				alloList[i] = FixMemoryAllocator( ( i + 1 ) * 4 );
		}
		assert( idx >= 0 && idx < MAX_SIZE );
		return alloList[idx];
	}
	static FixMemoryAllocator& get_allocator( unsigned int _size )
	{
		assert( _size > 0 );
		return at( ( _size - 1 ) >> 2 );
	}
	static void shrink_to_fit()
	{
		for( int i = 0; i < MAX_SIZE; i++ )
			at( i ).shrink_to_fit();
	}
};

template <class T>
struct low_fragmentation_allocator
{
	using value_type = T;
	low_fragmentation_allocator() noexcept
	{}
	template <class U> low_fragmentation_allocator( const low_fragmentation_allocator<U>& ) noexcept
	{}
	T* allocate( std::size_t n )
	{
		assert( n > 0 );
		auto len = n * sizeof( T );
		if( len <= FragmentManagement::MAX_SIZE * 4 )
			return reinterpret_cast<T*>( FragmentManagement::get_allocator( static_cast<unsigned int>( len ) ).allocate_untyped() );
		return reinterpret_cast<T*>( std::malloc( len ) );
	}
	void deallocate( T* p, std::size_t n )
	{
		assert( n > 0 );
		if( n <= FragmentManagement::MAX_SIZE * 4 )
			FragmentManagement::get_allocator( static_cast<unsigned int>( n ) ).deallocate_untyped( reinterpret_cast<std::byte*>( p ) );
		else
			std::free( p );
	}
};
template <class T, class U>
constexpr bool operator== ( const low_fragmentation_allocator<T>&, const low_fragmentation_allocator<U>& ) noexcept
{
	return true;
}
template <class T, class U>
constexpr bool operator!= ( const low_fragmentation_allocator<T>&, const low_fragmentation_allocator<U>& ) noexcept
{
	return false;
}
}