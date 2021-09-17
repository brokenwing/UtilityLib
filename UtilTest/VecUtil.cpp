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

TEST( VecUtil, LT_str )
{
	char s1[] = "abc";
	char s2[] = "abd";
	char s3[] = "aad";
	EXPECT_TRUE( LT( s1, s2, 3 ) );
	EXPECT_TRUE( !LT( s1, s1, 3 ) );
	EXPECT_TRUE( !LT( s1, s3, 3 ) );
}

TEST( VecUtil, LT_str_diff_type )
{
	std::string s1 = "abc";
	char s2[] = "abd";
	char s3[] = "aad";
	EXPECT_TRUE( LT( s1.begin(), s2, 3 ) );
	EXPECT_TRUE( !LT( s1.begin(), s1.begin(), 3 ) );
	EXPECT_TRUE( !LT( s1.begin(), s3, 3 ) );
}

TEST( VecUtil, GT_str )
{
	char s1[] = "abc";
	char s2[] = "abd";
	char s3[] = "aad";
	EXPECT_TRUE( !GT( s1, s2, 3 ) );
	EXPECT_TRUE( !GT( s1, s1, 3 ) );
	EXPECT_TRUE( GT( s1, s3, 3 ) );
}

TEST( VecUtil, GT_str_diff_type )
{
	std::string s1 = "abc";
	char s2[] = "abd";
	char s3[] = "aad";
	EXPECT_TRUE( !GT( s1.begin(), s2, 3 ) );
	EXPECT_TRUE( !GT( s1.begin(), s1.begin(), 3 ) );
	EXPECT_TRUE( GT( s1.begin(), s3, 3 ) );
}

TEST( VecUtil, LE_str )
{
	char s1[] = "abc";
	char s2[] = "abd";
	char s3[] = "aad";
	EXPECT_TRUE( LE( s1, s2, 3 ) );
	EXPECT_TRUE( LE( s1, s1, 3 ) );
	EXPECT_TRUE( !LE( s1, s3, 3 ) );
}

TEST( VecUtil, LE_str_diff_type )
{
	std::string s1 = "abc";
	char s2[] = "abd";
	char s3[] = "aad";
	EXPECT_TRUE( LE( s1.begin(), s2, 3 ) );
	EXPECT_TRUE( LE( s1.begin(), s1.begin(), 3 ) );
	EXPECT_TRUE( !LE( s1.begin(), s3, 3 ) );
}

TEST( VecUtil, GE_str )
{
	char s1[] = "abc";
	char s2[] = "abd";
	char s3[] = "aad";
	EXPECT_TRUE( !GE( s1, s2, 3 ) );
	EXPECT_TRUE( GE( s1, s1, 3 ) );
	EXPECT_TRUE( GE( s1, s3, 3 ) );
}

TEST( VecUtil, GE_str_diff_type )
{
	std::string s1 = "abc";
	char s2[] = "abd";
	char s3[] = "aad";
	EXPECT_TRUE( !GE( s1.begin(), s2, 3 ) );
	EXPECT_TRUE( GE( s1.begin(), s1.begin(), 3 ) );
	EXPECT_TRUE( GE( s1.begin(), s3, 3 ) );
}

TEST( VecUtil, VecAdd )
{
	int arr[3] = { 1,2,3 };
	int r[3] = { 2,3,4 };
	VecAdd( arr, arr + 3, 1 );
	EXPECT_TRUE( EQ( arr, r, 3 ) );
}

TEST( VecUtil, VecDec )
{
	int arr[3] = { 1,2,3 };
	int r[3] = { 0,1,2 };
	VecDec( arr, arr + 3, 1 );
	EXPECT_TRUE( EQ( arr, r, 3 ) );
}

TEST( VecUtil, VecMul )
{
	int arr[3] = { 1,2,3 };
	int r[3] = { 3,6,9 };
	VecMul( arr, arr + 3, 3 );
	EXPECT_TRUE( EQ( arr, r, 3 ) );
}

TEST( VecUtil, VecDiv )
{
	int arr[3] = { 2,4,6 };
	int r[3] = { 1,2,3 };
	VecDiv( arr, arr + 3, 2 );
	EXPECT_TRUE( EQ( arr, r, 3 ) );
}

TEST( VecUtil, VecMod )
{
	int arr[3] = { 2,4,6 };
	int r[3] = { 2,1,0 };
	VecMod( arr, arr + 3, 3 );
	EXPECT_TRUE( EQ( arr, r, 3 ) );
}