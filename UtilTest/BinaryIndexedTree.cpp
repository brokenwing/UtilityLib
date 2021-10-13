#include "pch.h"
#include "BinaryIndexedTree.h"

using namespace Util;

TEST( BinaryIndexedTree, basic_0 )
{
	BinaryIndexedTree tree( 10 );
	tree.Add( 0, 1 );
	EXPECT_EQ( tree[0], 1 );
}
TEST( BinaryIndexedTree, basic_n )
{
	BinaryIndexedTree tree( 10 );
	tree.Add( 9, 1 );
	EXPECT_EQ( tree[9], 1 );
}
TEST( BinaryIndexedTree, basic2 )
{
	BinaryIndexedTree tree( 10 );
	tree.Add( 5, 1 );
	tree.Add( 7, 2 );
	EXPECT_EQ( tree.SumInterval( 5, 7 ), 3 );
}
TEST( BinaryIndexedTree, basic3 )
{
	int n = 10;
	BinaryIndexedTree tree( n );
	FOR( i, 0, n )
		tree.Add( i, i );
	EXPECT_EQ( tree.SumInterval( 0, n - 1 ), 45 );
}