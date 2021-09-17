#include "pch.h"
#include "BlockList.h"
#include "VecUtil.h"

using namespace Util;

TEST( SolverLib_BlockList, ConstBeginEnd )
{
	const BlockList<int> arr = { 1,2,3 };
	auto x = arr.block_begin();
	int result[3] = { 1,2,3 };
	EXPECT_TRUE( EQ( x->begin(), result, 3 ) );
	EXPECT_EQ( ++x, arr.block_end() );
}

TEST( SolverLib_BlockList, BeginEnd )
{
	BlockList<int> arr = { 1,2,3 };
	auto x = arr.block_begin();
	int result[3] = { 1,2,3 };
	EXPECT_TRUE( EQ( x->begin(), result, 3 ) );
	EXPECT_EQ( ++x, arr.block_end() );
}

TEST( SolverLib_BlockList, Size )
{
	BlockList<int> arr = { 1,2,3 };
	EXPECT_EQ( arr.block_size(), 1 );
	EXPECT_EQ( arr.size(), 3 );
	EXPECT_FALSE( arr.empty() );
}

TEST( SolverLib_BlockList, Empty )
{
	BlockList<int> arr;
	EXPECT_EQ( arr.block_size(), 0 );
	EXPECT_EQ( arr.size(), 0 );
	EXPECT_TRUE( arr.empty() );
}

TEST( SolverLib_BlockList, 2D_InitializerList )
{
	BlockList<int> arr = { {1,2},{3,4},{5,6} };
	EXPECT_EQ( arr.block_size(), 3 );
	EXPECT_EQ( arr.size(), 6 );
	int r1[2] = { 1,2 };
	int r2[2] = { 3,4 };
	int r3[2] = { 5,6 };
	EXPECT_TRUE( EQ( arr.block_at( 0 )->begin(), r1, 2 ) );
	EXPECT_TRUE( EQ( arr.block_at( 1 )->begin(), r2, 2 ) );
	EXPECT_TRUE( EQ( arr.block_at( 2 )->begin(), r3, 2 ) );
	for( int i = 0; i < 6; i++ )
		EXPECT_EQ( arr.at( i ), i + 1 );
	for( int i = 0; i < 6; i++ )
		EXPECT_EQ( arr[i], i + 1 );
}

TEST( SolverLib_BlockList, Merge )
{
	BlockList<int> arr = { {1,2},{3,4},{5,6} };
	arr.merge( 1, 0 );
	EXPECT_EQ( arr.block_size(), 2 );
	int r[4] = { 1,2,3,4 };
	EXPECT_TRUE( EQ( arr.block_at( 0 )->begin(), r, 4 ) );
}

TEST( SolverLib_BlockList, Split )
{
	BlockList<int> arr = { {1,2},{3,4} };
	arr.split( 1 );
	EXPECT_EQ( arr.block_size(), 3 );
	int r1[1] = { 1 };
	int r2[1] = { 2 };
	EXPECT_TRUE( EQ( arr.block_at( 0 )->begin(), r1, 1 ) );
	EXPECT_TRUE( EQ( arr.block_at( 1 )->begin(), r2, 1 ) );
}

TEST( SolverLib_BlockList, PushBack )
{
	BlockList<int> arr = {};
	arr.push_back( 1 );
	arr.push_back( 2 );
	EXPECT_EQ( arr.block_size(), 1 );
	EXPECT_EQ( arr.size(), 2 );
	int r[2] = { 1,2 };
	EXPECT_TRUE( EQ( arr.block_at( 0 )->begin(), r, 2 ) );
}

TEST( SolverLib_BlockList, Move_behind )
{
	BlockList<int> arr = { {1,2},{3,4} };
	arr.move( 0, 1 );
	EXPECT_EQ( arr.block_size(), 2 );
	int r1[2] = { 3,4 };
	int r2[2] = { 1,2 };
	EXPECT_TRUE( EQ( arr.block_at( 0 )->begin(), r1, 2 ) );
	EXPECT_TRUE( EQ( arr.block_at( 1 )->begin(), r2, 2 ) );
}

TEST( SolverLib_BlockList, Move_before )
{
	BlockList<int> arr = { {1,2},{3,4} };
	arr.move( 1, 0 );
	EXPECT_EQ( arr.block_size(), 2 );
	int r1[2] = { 3,4 };
	int r2[2] = { 1,2 };
	EXPECT_TRUE( EQ( arr.block_at( 0 )->begin(), r1, 2 ) );
	EXPECT_TRUE( EQ( arr.block_at( 1 )->begin(), r2, 2 ) );
}

TEST( SolverLib_BlockList, Iterator_Forward )
{
	BlockList<int> arr = { {1,2},{3,4} };
	auto i = arr.begin();
	EXPECT_EQ( *i, 1 );
	++i;
	EXPECT_EQ( *i, 2 );
	++i;
	EXPECT_EQ( *i, 3 );
	++i;
	EXPECT_EQ( *i, 4 );
	++i;
	ASSERT_EQ( i, arr.end() );
}

TEST( SolverLib_BlockList, Iterator_Backward )
{
	BlockList<int> arr = { {1,2},{3,4} };
	auto i = arr.end();
	--i;
	EXPECT_EQ( *i, 4 );
	--i;
	EXPECT_EQ( *i, 3 );
	--i;
	EXPECT_EQ( *i, 2 );
	--i;
	EXPECT_EQ( *i, 1 );
	ASSERT_EQ( i, arr.begin() );
}

TEST( SolverLib_BlockList, Const_Iterator_Forward )
{
	const BlockList<int> arr = { {1,2},{3,4} };
	auto i = arr.begin();
	EXPECT_EQ( *i, 1 );
	++i;
	EXPECT_EQ( *i, 2 );
	++i;
	EXPECT_EQ( *i, 3 );
	++i;
	EXPECT_EQ( *i, 4 );
	++i;
	ASSERT_EQ( i, arr.end() );
}

TEST( SolverLib_BlockList, Const_Iterator_Backward )
{
	const BlockList<int> arr = { {1,2},{3,4} };
	auto i = arr.end();
	--i;
	EXPECT_EQ( *i, 4 );
	--i;
	EXPECT_EQ( *i, 3 );
	--i;
	EXPECT_EQ( *i, 2 );
	--i;
	EXPECT_EQ( *i, 1 );
	ASSERT_EQ( i, arr.begin() );
}

TEST( SolverLib_BlockList, Const_Iterator_to_Iterator )
{
	BlockList<int> arr = { {1,2},{3,4} };
	BlockList<int>::iterator x = arr.begin();
	BlockList<int>::const_iterator y = x;
	EXPECT_EQ( *x, *y );
}
