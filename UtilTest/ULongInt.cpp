#include "pch.h"
#include "ULongInt.h"

using namespace Util;

TEST( ULongInt, add )
{
	ULongInt a( 1 );
	ULongInt b( 2 );
	ULongInt c( 3 );
	EXPECT_EQ( a + b, c );
}
TEST( ULongInt, add2 )
{
	ULongInt a( 1 );
	ULongInt b( 2 );
	ULongInt c( 3 );
	a += b;
	EXPECT_EQ( a, c );
}
TEST( ULongInt, sub )
{
	ULongInt a( 2 );
	ULongInt b( 1 );
	ULongInt c( 1 );
	EXPECT_EQ( a - b, c );
}
TEST( ULongInt, sub2 )
{
	ULongInt a( 2 );
	ULongInt b( 1 );
	ULongInt c( 1 );
	a -= b;
	EXPECT_EQ( a, c );
}
TEST( ULongInt, mul )
{
	ULongInt a( 3 );
	ULongInt b( 4 );
	ULongInt c( 12 );
	EXPECT_EQ( a * b, c );
}
TEST( ULongInt, mul2 )
{
	ULongInt a( 3 );
	ULongInt b( 4 );
	ULongInt c( 12 );
	a *= b;
	EXPECT_EQ( a, c );
}
TEST( ULongInt, div )
{
	ULongInt a( 11 );
	ULongInt b( 4 );
	ULongInt c( 2 );
	EXPECT_EQ( a / b, c );
}
TEST( ULongInt, div2 )
{
	ULongInt a( 11 );
	ULongInt b( 4 );
	ULongInt c( 2 );
	a /= b;
	EXPECT_EQ( a, c );
}
TEST( ULongInt, mod )
{
	ULongInt a( 11 );
	ULongInt b( 4 );
	ULongInt c( 3 );
	EXPECT_EQ( a % b, c );
}
TEST( ULongInt, mod2 )
{
	ULongInt a( 11 );
	ULongInt b( 4 );
	ULongInt c( 3 );
	a %= b;
	EXPECT_EQ( a, c );
}
TEST( ULongInt, eq )
{
	ULongInt a( 3 );
	ULongInt b( 3 );
	ULongInt c( 5 );
	EXPECT_TRUE( a == b );
	EXPECT_TRUE( !( a == c ) );
}
TEST( ULongInt, neq )
{
	ULongInt a( 3 );
	ULongInt b( 3 );
	ULongInt c( 5 );
	EXPECT_TRUE( !( a != b ) );
	EXPECT_TRUE( a != c );
}
TEST( ULongInt, exp )
{
	ULongInt a( 3 );
	ULongInt c( 27 );
	EXPECT_TRUE( (a ^ 3) == c );
}
TEST( ULongInt, exp2 )
{
	ULongInt a( 3 );
	ULongInt c( 27 % 10 );
	EXPECT_TRUE( a.PowerMod( 3, 10 ) == c );
}