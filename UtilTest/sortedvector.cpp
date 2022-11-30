#include "pch.h"
#include "sortedvector.h"
#include <set>
#include <random>

namespace
{
template <typename ContainerA, typename ContainerB>
bool is_same_tree( const ContainerA& tree, const ContainerB& me )
{
	if( tree.size() != me.size() )
		return false;
	auto x = tree.begin();
	auto y = me.begin();
	while( x != tree.end() && y != me.end() )
	{
		if( *x != *y )
			return false;
		++x;
		++y;
	}
	if( x != tree.end() || y != me.end() )
		return false;
	return true;
}
}

TEST( sortedvector, compile_basic )
{
	Util::sortedvectorSet<int> vec;
	vec.check();
	vec.insert(0);
	vec.find_by_rank( 1 );
	vec.find( 0 );
	vec.erase( 0 );
	vec.erase( vec.begin(), vec.end() );
	auto a = vec.begin();
	auto b = vec.end();
	auto c = vec.empty();
	auto d = vec.size();
	vec.clear();
	vec.reserve( 1 );

	const auto& cvec = vec;
	cvec.begin();
	cvec.end();
	cvec.find( 0 );
}

TEST( sortedvector, compile_struct )
{
	struct A
	{
		int me = 0;
		bool operator<( A b )const
		{
			return me < b.me;
		}
	};
	A elm;
	Util::sortedvectorSet<A> vec;
	vec.check();
	vec.insert( elm );
	vec.find_by_rank( 1 );
	vec.find( elm );
	vec.erase( elm );
	vec.erase( vec.begin(), vec.end() );
	auto a = vec.begin();
	auto b = vec.end();
	auto c = vec.empty();
	auto d = vec.size();
	vec.clear();
	vec.reserve( 1 );
	const auto& cvec = vec;
	cvec.begin();
	cvec.end();
	cvec.find( elm );
}

TEST( sortedvector, insert_easy )
{
	Util::sortedvectorSet<int> vec;
	vec.insert( 2 );
	vec.insert( 1 );
	vec.insert( 3 );
	EXPECT_TRUE( vec.check() );
	std::vector<int> tmp = vec.toVector();
	EXPECT_EQ( tmp, std::vector<int>( { 1,2,3 } ) );
}
TEST( sortedvector, insert_return )
{
	Util::sortedvectorSet<int> vec;
	vec.reserve( 10 );
	auto r1 = vec.insert( 2 );
	EXPECT_EQ( *r1, 2 );
	auto r2 = vec.insert( 1 );
	EXPECT_EQ( *r2, 1 );
	auto r3 = vec.insert( 3 );
	EXPECT_EQ( *r3, 3 );
}
TEST( sortedvector, erase_easy )
{
	Util::sortedvectorSet<int> vec;
	vec.reserve( 10 );
	vec.insert( 2 );
	vec.insert( 1 );
	vec.insert( 3 );
	vec.erase( 2 );
	EXPECT_TRUE( vec.check() );
	std::vector<int> tmp = vec.toVector();
	EXPECT_EQ( tmp, std::vector<int>( { 1,3 } ) );
}
TEST( sortedvector, find_easy )
{
	Util::sortedvectorSet<int> vec;
	vec.reserve( 10 );
	vec.insert( 2 );
	auto r = vec.find( 2 );
	EXPECT_TRUE( vec.check() );
	ASSERT_NE( r, vec.end() );
	EXPECT_EQ( *r, 2 );
}

TEST( sortedvectorMultiSet, insert_find_large )
{
	const int n = 100;
	const int l = 0;
	const int r = 35;
	Util::sortedvectorMultiSet<int> vec;
	vec.reserve( n + 10 );
	std::multiset<int> me;
	std::mt19937 rng( 0 );
	std::uniform_int_distribution<int> range( l, r );
	
	for( int i = 0; i < n; i++ )
	{
		const int val = range( rng );
		auto r = vec.insert( val );
		me.insert( val );
		//std::cout << i << ' ' << val << '\n';
		ASSERT_TRUE( vec.check() );
		ASSERT_EQ( *r, val );
		ASSERT_TRUE( is_same_tree( vec, me ) );
		{
			const int find_val = range( rng );
			auto x = vec.find( find_val );
			auto y = me.find( find_val );
			ASSERT_EQ( y == me.end(), x == vec.end() );
			if( x != vec.end() )
				ASSERT_EQ( *x, find_val );
		}
	}
}

TEST( sortedvector, insert_find_large_nodup )
{
	const int n = 100;
	const int l = 0;
	const int r = 35;
	Util::sortedvectorSet<int> vec;
	vec.reserve( n + 10 );
	std::set<int> me;
	std::mt19937 rng( 0 );
	std::uniform_int_distribution<int> range( l, r );
	
	for( int i = 0; i < n; i++ )
	{
		const int val = range( rng );
		auto r = vec.insert( val );
		me.insert( val );
		//std::cout << i << ' ' << val << '\n';
		ASSERT_TRUE( vec.check() );
		ASSERT_EQ( *r, val );
		ASSERT_TRUE( is_same_tree( vec, me ) );
		{
			const int find_val = range( rng );
			auto x = vec.find( find_val );
			auto y = me.find( find_val );
			ASSERT_EQ( y == me.end(), x == vec.end() );
			if( x != vec.end() )
				ASSERT_EQ( *x, find_val );
		}
	}
}

TEST( sortedvectorMap, compile_basic )
{
	Util::sortedvectorMap<int, std::string> vec;
	vec.check();
	vec.begin();
	vec.end();
	vec.insert( { 0,"" } );
	vec.find_by_rank( 1 );
	vec.find( 0 );
	vec.erase( 0 );
	vec.erase( vec.begin(), vec.end() );
	vec[0] = "";
	auto a = vec.begin();
	auto b = vec.end();
	auto c = vec.empty();
	auto d = vec.size();
	vec.clear();
	vec.reserve( 1 );

	const auto& cvec = vec;
	cvec.begin();
	cvec.end();
	cvec.find( 0 );
}
TEST( sortedvectorMap, basic )
{
	Util::sortedvectorMap<int, int> vec;
	vec[-1] = 1;
	vec[1] = -1;
	EXPECT_EQ( vec.find( -1 )->value(), 1 );
	EXPECT_EQ( vec.find( 1 )->value(), -1 );
	EXPECT_EQ( vec.size(), 2 );
	vec.erase( vec.begin() );
	EXPECT_EQ( vec.size(), 1 );
	EXPECT_EQ( vec.find( -1 ), vec.end() );
	vec[1] = 2;
	EXPECT_EQ( vec.begin()->value(), 2 );
}