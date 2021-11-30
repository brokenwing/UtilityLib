#include "pch.h"
#include "../UtilityLib/Util.h"
#include "../UtilityLib/defrag_memory_resource.h"

using namespace std::pmr;
using namespace Util;

TEST( defrag_memory_resource, normal_use )
{
	defrag_memory_resource allo;
	vector<int> arr( &allo );
	const int n = 100;
	FOR( i, 0, n )
		arr.emplace_back( i );
	FOR( i, 0, n )
		EXPECT_EQ( arr[i], i );
}
TEST( defrag_memory_resource, normal_use_reserve )
{
	defrag_memory_resource allo;
	vector<int> arr( &allo );
	const int n = 100;
	arr.reserve( n * 2 );
	FOR( i, 0, n )
		arr.emplace_back( i );
	FOR( i, 0, n )
		EXPECT_EQ( arr[i], i );
}
TEST( defrag_memory_resource, normal_use_list )
{
	defrag_memory_resource allo;
	list<int> arr( &allo );
	const int n = 100;
	FOR( i, 0, n )
		arr.emplace_back( i );
	int idx = 0;
	for( auto& e : arr )
		EXPECT_EQ( e, idx++ );
}
TEST( defrag_memory_resource, defrag_map )
{
	defrag_memory_resource allo( 0, 250 );
	map<int, int> arr( &allo );
	const int n = 4;
	FOR( i, 0, n )
		arr[i] = i;
	ASSERT_NO_THROW( allo.defragging( arr ) );
	allo.switch_buffer();
	allo.release_other_buffer();
	const map<int, int>::value_type* before = nullptr;
	for( auto& e : arr )
	{
		if( before )
			EXPECT_LE( abs( &e - before ), 20 );
		before = &e;
	}
	ASSERT_NO_THROW( arr.clear() );
}
TEST( defrag_memory_resource, defrag_set )
{
	defrag_memory_resource allo( 0, 250 );
	set<int> arr( &allo );
	const int n = 4;
	FOR( i, 0, n )
		arr.emplace( i );
	ASSERT_NO_THROW( allo.defragging( arr ) );
	allo.switch_buffer();
	allo.release_other_buffer();
	const int* before = nullptr;
	for( auto& e : arr )
	{
		if( before )
			EXPECT_LE( abs( &e - before ), 20 );
		before = &e;
	}
	ASSERT_NO_THROW( arr.clear() );
}
TEST( defrag_memory_resource, defrag_basic_list )
{
	defrag_memory_resource allo( 0, 200 );
	list<int> arr( &allo );//data 4byte + prev pointer 8 byte + next pointer 8 byte
	const int n = 4;
	FOR( i, 0, n )
		arr.emplace_back( i );
	ASSERT_NO_THROW( allo.defragging( arr ) );
	allo.switch_buffer();
	allo.release_other_buffer();
	int* before = nullptr;
	for( auto& e : arr )
	{
		if( before )
			EXPECT_LE( abs( &e - before ), 12 );
		before = &e;
	}
	ASSERT_NO_THROW( arr.clear() );
}
TEST( defrag_memory_resource, defrag_large_vector )
{
	defrag_memory_resource allo( 10, 200 );
	list<vector<int>> arr( &allo );
	const int n = 10;
	FOR( i, 0, n )
	{
		auto& e = arr.emplace_back( vector<int>( &allo ) );
		FOR( j, 0, n )
			e.emplace_back( i * n + j );
	}
	int idx = 0;
	for( auto& e1 : arr )
		for( auto& e2 : e1 )
			EXPECT_EQ( e2, idx++ );
	ASSERT_NO_THROW( allo.defragging( arr ) );
	allo.switch_buffer();
	allo.release_other_buffer();
	idx = 0;
	for( auto& e1 : arr )
		for( auto& e2 : e1 )
		{
			EXPECT_EQ( e2, idx++ );
			ASSERT_NO_THROW( e2 = -1 );
		}
	ASSERT_NO_THROW( arr.clear() );
}