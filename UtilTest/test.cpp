#include "pch.h"
#include "Util.h"
#include "CommonDef.h"

using namespace Util;

constexpr double EPS_UP = 1.1;
constexpr double EPS_LOW = 0.9;

TEST( Util, IsZero )
{
	EXPECT_TRUE( IsZero( 0 ) );
	EXPECT_TRUE( IsZero( eps * EPS_LOW ) );
	EXPECT_TRUE( !IsZero( eps * EPS_UP ) );
}
TEST( Util, LT )
{
	EXPECT_TRUE( LT( 1, 2 ) );
	EXPECT_TRUE( !LT( 1, 1 ) );
	EXPECT_TRUE( LT( 1, 1 + eps * EPS_UP ) );
	EXPECT_TRUE( !LT( 1, 1 + eps * EPS_LOW ) );
}
TEST( Util, GT )
{
	EXPECT_TRUE( GT( 2, 1 ) );
	EXPECT_TRUE( !GT( 1, 1 ) );
	EXPECT_TRUE( GT( 1 + eps * EPS_UP, 1 ) );
	EXPECT_TRUE( !GT( 1 + eps * EPS_LOW, 1 ) );
}
TEST( Util, LE )
{
	EXPECT_TRUE( LE( 1, 2 ) );
	EXPECT_TRUE( LE( 1, 1 ) );
	EXPECT_TRUE( LE( 1, 1 - eps * EPS_LOW ) );
	EXPECT_TRUE( !LE( 1, 1 - eps * EPS_UP ) );
}
TEST( Util, GE )
{
	EXPECT_TRUE( GE( 2, 1 ) );
	EXPECT_TRUE( GE( 1, 1 ) );
	EXPECT_TRUE( GE( 1, 1 + eps * EPS_LOW ) );
	EXPECT_TRUE( !GE( 1, 1 + eps * EPS_UP ) );
}

TEST( Util, Floor_int )
{
	EXPECT_EQ( Floor( 120, 100 ), 100 );
	EXPECT_EQ( Floor( -120, 100 ), -200 );
	EXPECT_EQ( Floor( 10, 10 ), 10 );
	EXPECT_EQ( Floor( -10, 10 ), -10 );
	EXPECT_EQ( Floor( 0, 10 ), 0 );
}

TEST( Util, Ceil_int )
{
	EXPECT_EQ( Ceil( 120, 100 ), 200 );
	EXPECT_EQ( Ceil( -120, 100 ), -100 );
	EXPECT_EQ( Ceil( 10, 10 ), 10 );
	EXPECT_EQ( Ceil( -10, 10 ), -10 );
	EXPECT_EQ( Ceil( 0, 10 ), 0 );
}

TEST( Util, Floor_double )
{
	EXPECT_DOUBLE_EQ( Floor( 10 ), 10 );
	EXPECT_DOUBLE_EQ( Floor( 10.5 ), 10 );
	EXPECT_DOUBLE_EQ( Floor( 10 - eps * EPS_LOW ), 10 );
	EXPECT_DOUBLE_EQ( Floor( 10 - eps * EPS_UP ), 9 );
}

TEST( Util, Ceil_double )
{
	EXPECT_DOUBLE_EQ( Ceil( 10 ), 10 );
	EXPECT_DOUBLE_EQ( Ceil( 10.5 ), 11 );
	EXPECT_DOUBLE_EQ( Ceil( 10 + eps * EPS_LOW ), 10 );
	EXPECT_DOUBLE_EQ( Ceil( 10 + eps * EPS_UP ), 11 );
}

TEST( Util, SHL )
{
	EXPECT_EQ( shl( 0b001, 1 ), 0b010 );
	EXPECT_EQ( shl( 0b0010, 1 ), 0b0100 );
	EXPECT_EQ( shl( 0b011, 1 ), 0b110 );
	EXPECT_EQ( shl( 0b0101, 1 ), 0b1010 );
	EXPECT_EQ( shl( 0b0001, 3 ), 0b1000 );
	EXPECT_EQ( shl<std::uint8_t>( 0b10000000, 1 ), 1 );
	EXPECT_EQ( shl<int>( 0b10000000, 1 ), 0b100000000 );
	EXPECT_EQ( shl<int>( 11, 32 * 3 ), 11 );
	EXPECT_EQ( shl( 0x0321, 4 ), 0x3210 );
}

TEST( Util, SHR )
{
	EXPECT_EQ( shr( 0b010, 1 ), 0b001 );
	EXPECT_EQ( shr( 0b0100, 1 ), 0b0010 );
	EXPECT_EQ( shr( 0b110, 1 ), 0b011 );
	EXPECT_EQ( shr( 0b1010, 1 ), 0b0101 );
	EXPECT_EQ( shr( 0b1000, 3 ), 0b0001 );
	EXPECT_EQ( shr<std::uint8_t>( 1, 1 ), 0b10000000 );
	EXPECT_EQ( shr<int>( 0b100000000, 1 ), 0b10000000 );
	EXPECT_EQ( shr<int>( 11, 32 * 3 ), 11 );
	EXPECT_EQ( shr( 0x3210, 4 ), 0x0321 );
}

TEST( Util, XORswap_int )
{
	int a = 1, b = 2;
	XORswap( a, b );
	EXPECT_EQ( a, 2 );
	EXPECT_EQ( b, 1 );
}

TEST( Util_Move, R_0_1 )
{
	const int target[5] = { 2,1,3,4,5 };
	int arr[5] = { 1,2,3,4,5 };
	Move( arr + 0, arr + 1 );
	for( int i = 0; i < 5; i++ )
		EXPECT_EQ( arr[i], target[i] );
}

TEST( Util_Move, R_1_3 )
{
	const int target[5] = { 1,3,4,2,5 };
	int arr[5] = { 1,2,3,4,5 };
	Move( arr + 1, arr + 3 );
	for( int i = 0; i < 5; i++ )
		EXPECT_EQ( arr[i], target[i] );
}

TEST( Util_Move, L_1_0 )
{
	const int target[5] = { 2,1,3,4,5 };
	int arr[5] = { 1,2,3,4,5 };
	Move( arr + 1, arr + 0 );
	for( int i = 0; i < 5; i++ )
		EXPECT_EQ( arr[i], target[i] );
}

TEST( Util_Move, L_3_1 )
{
	const int target[5] = { 1,4,2,3,5 };
	int arr[5] = { 1,2,3,4,5 };
	Move( arr + 3, arr + 1 );
	for( int i = 0; i < 5; i++ )
		EXPECT_EQ( arr[i], target[i] );
}

TEST( Util, ToString )
{
	auto s1 = ToString( 1, 2, 3 );
	EXPECT_EQ( s1, "123" );
	auto s2 = ToString( 1, "23", 4, '5' );
	EXPECT_EQ( s2, "12345" );
}

TEST( Util, ToWString )
{
	auto s1 = ToWString( 1, 2, 3 );
	EXPECT_EQ( s1, L"123" );
	auto s2 = ToWString( 1, L"23", 4, L'5' );
	EXPECT_EQ( s2, L"12345" );
}

TEST( Println, Check_123 )
{
	EXPECT_NO_THROW( Println( 1, '2', "3" ) );
}

TEST( PrintVec, Check_123 )
{
	std::vector<int> q = { 1,2,3 };
	EXPECT_NO_THROW( PrintVec( q.begin(), q.end() ) );
}