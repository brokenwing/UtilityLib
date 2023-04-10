#include "pch.h"
#include "DisjointSet.h"

using namespace Util;

TEST( DisjointSet, basic )
{
	DisjointSet ds;
	ds.Merge( 1, 2 );
	ds.Merge( 3, 4 );
	EXPECT_EQ( ds.GetRoot( 1 ), ds.GetRoot( 2 ) );
	EXPECT_EQ( ds.GetRoot( 3 ), ds.GetRoot( 4 ) );
	ds.Merge( 1,4 );
	EXPECT_EQ( ds.GetRoot( 1 ), ds.GetRoot( 3 ) );
	EXPECT_EQ( ds.GetRoot( 1 ), ds.GetRoot( 4 ) );
	EXPECT_EQ( ds.GetRoot( 2 ), ds.GetRoot( 3 ) );
	EXPECT_EQ( ds.GetRoot( 2 ), ds.GetRoot( 4 ) );
}
TEST( DisjointSet, count )
{
	DisjointSet ds;
	ds.Merge( 1, 2 );
	ds.Merge( 3, 4 );
	ds.Merge( 5, 6 );
	EXPECT_EQ( ds.Count(), 3 );
	ds.Merge( 1,4 );
	EXPECT_EQ( ds.Count(), 2 );
	ds.Merge( 3,5 );
	EXPECT_EQ( ds.Count(), 1 );
}
TEST( DisjointSet, size )
{
	DisjointSet ds;
	ds.Merge( 1, 2 );
	EXPECT_EQ( ds.size(), 2 );
	ds.Merge( 3, 4 );
	EXPECT_EQ( ds.size(), 4 );
	ds.Merge( 5, 6 );
	EXPECT_EQ( ds.size(), 6 );
	ds.Merge( 1, 7 );
	EXPECT_EQ( ds.size(), 7 );
}