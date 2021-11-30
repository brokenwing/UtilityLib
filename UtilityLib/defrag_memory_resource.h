#pragma once
#include <memory_resource>

namespace Util
{
class defrag_memory_resource :public std::pmr::_Identity_equal_resource
{
public:
	using buffer_type = std::pmr::monotonic_buffer_resource;
	using buffer_ptr = std::unique_ptr<buffer_type>;

private:
	bool idx = 0;
	buffer_ptr buf[2];
	//deallocated position is uncertain, only allocated size
	size_t total_size_hisotry[2] = { 0 };

public:
	defrag_memory_resource( size_t initial_size1 = 0, size_t initial_size2 = 0 ) :
		defrag_memory_resource( std::make_unique<buffer_type>( initial_size1 ), std::make_unique<buffer_type>( initial_size2 ) )
	{}
	defrag_memory_resource( buffer_ptr buf1, buffer_ptr buf2 = buffer_ptr() )
	{
		buf[idx] = std::move( buf1 );
		buf[idx ^ 1] = std::move( buf2 );
	}
	~defrag_memory_resource()
	{}
	defrag_memory_resource( const defrag_memory_resource& ) = delete;

	virtual void* do_allocate( size_t _Bytes, size_t _Align )               override
	{
		total_size_hisotry[idx] += _Bytes;
		return buf[idx]->allocate( _Bytes, _Align );
	}
	//do nothing
	virtual void do_deallocate( void* _Ptr, size_t _Bytes, size_t _Align )  override
	{}

	void release_other_buffer()
	{
		buf[idx ^ 1]->release();
	}
	constexpr void switch_buffer()noexcept
	{
		idx ^= 1;
	}
	void reset_buffer( buffer_ptr p )
	{
		buf[idx] = std::move( p );
	}
	void reset_buffer( size_t initial_size )
	{
		buf[idx] = std::make_unique<buffer_type>( initial_size );
	}
	constexpr void reset_allocated_size_history()noexcept
	{
		total_size_hisotry[idx] = 0;
	}
	constexpr size_t get_allocated_size_history()const noexcept
	{
		return total_size_hisotry[idx];
	}

	template<typename Container>
	void defragging( Container& val )
	{
		if( val.get_allocator().resource() == this )
		{
			switch_buffer();
			Container tmp( this );
			tmp.swap( val );
			val = tmp;
			switch_buffer();
		}
	}
};
}