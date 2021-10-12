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

TEST( VecUtil, isAscend )
{
	int a[3] = { 1,2,3 };
	int b[3] = { 1,2,2 };
	int c[3] = { 1,2,1 };
	EXPECT_TRUE( isAscend( a, a + 3 ) );
	EXPECT_TRUE( !isAscend( b, b + 3 ) );
	EXPECT_TRUE( !isAscend( c, c + 3 ) );
}

TEST( VecUtil, isAscendEQ )
{
	int a[3] = { 1,2,3 };
	int b[3] = { 1,2,2 };
	int c[3] = { 1,2,1 };
	EXPECT_TRUE( isAscendOrEqual( a, a + 3 ) );
	EXPECT_TRUE( isAscendOrEqual( b, b + 3 ) );
	EXPECT_TRUE( !isAscendOrEqual( c, c + 3 ) );
}

TEST( VecUtil, isDescend )
{
	int a[3] = { 3,2,1 };
	int b[3] = { 3,2,2 };
	int c[3] = { 3,2,4 };
	EXPECT_TRUE( isDescend( a, a + 3 ) );
	EXPECT_TRUE( !isDescend( b, b + 3 ) );
	EXPECT_TRUE( !isDescend( c, c + 3 ) );
}

TEST( VecUtil, isDescendEQ )
{
	int a[3] = { 3,2,1 };
	int b[3] = { 3,2,2 };
	int c[3] = { 3,2,4 };
	EXPECT_TRUE( isDescendOrEqual( a, a + 3 ) );
	EXPECT_TRUE( isDescendOrEqual( b, b + 3 ) );
	EXPECT_TRUE( !isDescendOrEqual( c, c + 3 ) );
}

TEST( VecUtil, isSame )
{
	int a[3] = { 1,1,1 };
	int b[3] = { 1,2,1 };
	EXPECT_TRUE( isSame( a, a + 3 ) );
	EXPECT_TRUE( !isSame( b, b + 3 ) );
}

TEST( VecUtil, VecCount )
{
	int arr[] = { 1,1,1,2,3,3 };
	int n = 6;
	std::vector<std::pair<int, int>> r = { {1,3},{2,1},{3,2} };
	auto q = VecCount2pair<int>( arr, arr + n );
	ASSERT_EQ( q.size(), r.size() );
	EXPECT_TRUE( EQ( r.begin(), q.begin(), r.size() ) );
}
TEST( VecUtil, VecCount2 )
{
	int arr[] = { 1,1,1,2,3,3 };
	int n = 6;
	std::vector<int> r = { 3,1,2 };
	auto q = VecCount( arr, arr + n );
	ASSERT_EQ( q.size(), r.size() );
	EXPECT_TRUE( EQ( r.begin(), q.begin(), r.size() ) );
}
TEST( VecUtil, VecCount2_func )
{
	int arr[] = { 1,3,5,2,3,1 };
	int n = 6;
	std::vector<int> r = { 3,1,2 };
	auto q = VecCount( arr, arr + n, [] ( int a, int b )
	{
		return a % 2 == b % 2;
	} );
	ASSERT_EQ( q.size(), r.size() );
	EXPECT_TRUE( EQ( r.begin(), q.begin(), r.size() ) );
}

TEST( VecUtil, VecCount_empty )
{
	int arr[] = { 1,2,3 };
	auto q = VecCount2pair<int>( arr, arr + 0 );
	EXPECT_TRUE( q.empty() );
}

TEST( VecUtil, VecCount_same )
{
	int arr[] = { 1,2,3 };
	auto q = VecCount2pair<int>( arr, arr + 3 );
	ASSERT_EQ( q.size(), 3 );
	for( int i = 0; i < 3; i++ )
		EXPECT_EQ( q[i], std::make_pair( i + 1, 1 ) );
}

TEST( VecHash, check_1 )
{
	int n = 1;
	int arr[] = { 1 };
	auto r = VecHash<int>( arr, arr + n );
	EXPECT_EQ( r, std::hash<int>()( 1 ) );
}

TEST( VecHash, check_stable )
{
	int n = 3;
	int arr[] = { 1,2,3 };
	auto r1 = VecHash<int>( arr, arr + n );
	auto r2 = VecHash<int>( arr, arr + n );
	EXPECT_EQ( r1, r2 );
}

TEST( VecHash, small_conflict_test )
{
	std::set<size_t> count;
	int n = 5;
	int len = 5;
	FOR( i, 0, n )
		FOR( j, 0, n )
			FOR( k, 0, n )
				FOR( a, 0, n )
					FOR( b, 0, n )
	{
		int arr[] = { i,j,k,a,b };
		auto r = VecHash<int>( arr, arr + len );
		count.insert( r );
	}
	EXPECT_EQ( count.size(), n* n* n* n* n );
}

TEST( SubVec, pick_all )
{
	int arr[] = { 1,2,3,4 };
	int idx[] = { 0,1,2,3 };
	int result[4] = { 0 };
	int check[] = { 1,2,3,4 };
	SubVec( arr, idx, result, 4 );
	EXPECT_TRUE( EQ( result, check, 4 ) );
}

TEST( SubVec, pick_partial )
{
	int arr[] = { 1,2,3,4 };
	int idx[] = { 1,2 };
	int result[4] = { 0 };
	int check[] = { 2,3 };
	SubVec( arr, idx, result, 2 );
	EXPECT_TRUE( EQ( result, check, 2 ) );
}

TEST( SubVec, pick_repeat )
{
	int arr[] = { 1,2,3,4 };
	int idx[] = { 0,0,0 };
	int result[4] = { 0 };
	int check[] = { 1,1,1 };
	SubVec( arr, idx, result, 3 );
	EXPECT_TRUE( EQ( result, check, 3 ) );
}

TEST( GenerateVec, n )
{
	auto r1 = GenerateVec( 4 );
	int r2[] = { 0,1,2,3 };
	EXPECT_TRUE( EQ( r1.begin(), r2, 4 ) );
}
TEST( GenerateVec, n_st )
{
	auto r1 = GenerateVec( 4, 1 );
	int r2[] = { 1,2,3,4 };
	EXPECT_TRUE( EQ( r1.begin(), r2, 4 ) );
}
TEST( GenerateVec, n_st_step )
{
	auto r1 = GenerateVec( 4, 0, 2 );
	int r2[] = { 0,2,4,6 };
	EXPECT_TRUE( EQ( r1.begin(), r2, 4 ) );
}

TEST( VecCompare, basic )
{
	int n = 4;
	int a[] = { 1,2,3,4 };
	int b[] = { 1,2,4,5 };
	int c[] = { 1,2,2,1 };
	EXPECT_EQ( Compare( a, b, n ), -1 );
	EXPECT_EQ( Compare( a, c, n ), 1 );
	EXPECT_EQ( Compare( a, a, n ), 0 );
}