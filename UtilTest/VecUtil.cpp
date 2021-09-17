#include "pch.h"
#include "VecUtil.h"

using namespace Util;

TEST( VecUtil, EQ )
{
	int a[3] = { 1,2,3 };
	int b[3] = { 1,2,3 };
	int c[3] = { 1,3,2 };
	EXPECT_TRUE( EQ( a, b, 3 ) );
	EXPECT_TRUE( !EQ( a, c, 2 ) );
}

TEST( VecUtil, EQ_diff_type )
{
	int a[3] = { 1,2,3 };
	std::vector<int> b = { 1,2,3 };
	std::vector<int> c = { 1,2,2 };
	EXPECT_TRUE( EQ( a, b.begin(), 3 ) );
	EXPECT_TRUE( !EQ( a, c.begin(), 3 ) );
}

TEST( VecUtil, NEQ )
{
	int a[3] = { 1,2,3 };
	int b[3] = { 1,2,3 };
	int c[3] = { 1,3,2 };
	EXPECT_TRUE( !NEQ( a, b, 3 ) );
	EXPECT_TRUE( NEQ( a, c, 2 ) );
}

TEST( VecUtil, NEQ_diff_type )
{
	int a[3] = { 1,2,3 };
	std::vector<int> b = { 1,2,3 };
	std::vector<int> c = { 1,2,2 };
	EXPECT_TRUE( !NEQ( a, b.begin(), 3 ) );
	EXPECT_TRUE( NEQ( a, c.begin(), 3 ) );
}