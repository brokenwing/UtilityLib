#include "pch.h"
#include "low_fragmentation_allocator.h"
#include <vector>
#include <set>
#include <random>

namespace
{
class OpenFixMemoryAllocator :public LFA::FixMemoryAllocator
{
public:
	using LFA::FixMemoryAllocator::FixMemoryAllocator;

	const decltype( pages )& get_pages()const
	{
		return pages;
	}
	const decltype( free )& get_free_space()const
	{
		return free;
	}
};
}

TEST( FixMemoryAllocator, basic )
{
	using namespace LFA;
	FixMemoryAllocator x( 4, 1000 );
	std::byte* p = nullptr;
	ASSERT_NO_THROW( p = x.allocate_untyped() );
	ASSERT_NO_THROW( x.deallocate_untyped( p ) );
	ASSERT_NO_THROW( x.~FixMemoryAllocator() );
}
TEST( FixMemoryAllocator, newpage )
{
	using namespace LFA;
	FixMemoryAllocator x( 4, 1000 );
	ASSERT_NO_THROW( x.newpage() );
}
TEST( FixMemoryAllocator, setvalue )
{
	using namespace LFA;
	FixMemoryAllocator x( 8, 1000 );
	double* v1 = reinterpret_cast<double*>( x.allocate_untyped() );
	double* v2 = reinterpret_cast<double*>( x.allocate_untyped() );
	double* v3 = reinterpret_cast<double*>( x.allocate_untyped() );
	*v1 = 2.5;
	*v2 = 3;
	*v3 = *v1 * *v2;
	EXPECT_DOUBLE_EQ( *v1, 2.5 );
	EXPECT_DOUBLE_EQ( *v2, 3 );
	EXPECT_DOUBLE_EQ( *v3, 7.5 );
	ASSERT_NO_THROW( x.deallocate_untyped( reinterpret_cast<std::byte*>( v1 ) ) );
	ASSERT_NO_THROW( x.deallocate_untyped( reinterpret_cast<std::byte*>( v2 ) ) );
	ASSERT_NO_THROW( x.deallocate_untyped( reinterpret_cast<std::byte*>( v3 ) ) );
}
TEST( FixMemoryAllocator, shrink_empty_page )
{
	using namespace LFA;
	OpenFixMemoryAllocator x( 4, 100 );
	x.newpage();
	x.shrink_to_fit();
	EXPECT_EQ( x.get_pages().size(), 0 );
}
TEST( FixMemoryAllocator, shrink_nonempty_page )
{
	using namespace LFA;
	OpenFixMemoryAllocator x( 4, 100 );
	x.newpage();
	auto p = x.allocate_untyped();
	ASSERT_NO_THROW( x.shrink_to_fit() );
	ASSERT_NO_THROW( x.deallocate_untyped( p ) );
	EXPECT_EQ( x.get_pages().size(), 1 );
}
TEST( FixMemoryAllocator, shrink_many_page )
{
	using namespace LFA;
	OpenFixMemoryAllocator x( 4, 4 );
	auto a = x.allocate_untyped();
	auto b = x.allocate_untyped();
	auto c = x.allocate_untyped();
	x.shrink_to_fit();
	EXPECT_EQ( x.get_pages().size(), 3 );
	x.deallocate_untyped( b );
	x.deallocate_untyped( c );
	x.shrink_to_fit();
	EXPECT_EQ( x.get_pages().size(), 1 );
	x.deallocate_untyped( a );
	x.shrink_to_fit();
	EXPECT_EQ( x.get_pages().size(), 0 );
}

TEST( lfa, set )
{
	using namespace LFA;
	FragmentManagement::shrink_to_fit();
	std::set<int, std::less<>, low_fragmentation_allocator<int>> x;
	ASSERT_NO_THROW( x.insert( 1 ) );
	ASSERT_NO_THROW( x.insert( 2 ) );
	ASSERT_NO_THROW( x.erase( 2 ) );
	ASSERT_NO_THROW( x.erase( 1 ) );
}
TEST( lfa, big )
{
	using namespace LFA;
	FragmentManagement::shrink_to_fit();
	std::vector<int, low_fragmentation_allocator<int>> x;
	ASSERT_NO_THROW( x.resize(100000) );
}
TEST( lfa, random_insert_delete )
{
	using namespace LFA;
	FragmentManagement::shrink_to_fit();
	std::set<int, std::less<>, low_fragmentation_allocator<int>> x;
	const int n = 50;
	std::uniform_int_distribution<int> gen( 0, 10 );
	std::mt19937 rng( 0 );
	for( int k = 0; k < 5; k++ )
	{
		for( int i = 0; i < n; i++ )
			ASSERT_NO_THROW( x.insert( gen( rng ) ) );
		for( int i = 0; i < n; i++ )
			ASSERT_NO_THROW( x.erase( gen( rng ) ) );
	}
}