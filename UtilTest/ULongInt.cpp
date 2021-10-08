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

TEST( ULongInt, hybrid_compute )
{
	ULongInt a( "1 23456789 01234619" );
	ULongInt b( "56789 01234619" );
	a *= a + 1;
	auto x = a / b;
	auto y = a % b;
	EXPECT_EQ( a, x * b + y );
}

TEST( ULongInt, hybrid_compute2 )
{
	ULongInt a( "132213" );
	ULongInt b( "619" );
	auto x = a / b;
	auto y = a % b;
	EXPECT_EQ( a, x * b + y );
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
TEST( ULongInt, lt )
{
	ULongInt a( 3 );
	ULongInt b( 4 );
	EXPECT_TRUE( a < b );
	EXPECT_FALSE( a < a );
}
TEST( ULongInt, le )
{
	ULongInt a( 3 );
	ULongInt b( 4 );
	EXPECT_TRUE( a <= b );
	EXPECT_TRUE( a <= a );
}
TEST( ULongInt, gt )
{
	ULongInt a( 3 );
	ULongInt b( 4 );
	EXPECT_TRUE( b > a );
	EXPECT_FALSE( a > a );
}
TEST( ULongInt, ge )
{
	ULongInt a( 3 );
	ULongInt b( 4 );
	EXPECT_TRUE( b >= a );
	EXPECT_TRUE( a >= a );
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
TEST( ULongInt, exp3 )
{
	ULongInt a( 101 );
	ULongInt c( 644214 );
	EXPECT_TRUE( a.PowerMod( 10, 1234567 ) == c );
}
TEST( ULongInt, exp4 )
{
	ULongInt a( "802 46248239 57136044" );
	ULongInt b( "617 28394506 17280791" );
	ULongInt n( "1234 56789012 34561583" );
	EXPECT_TRUE( a.PowerMod( b, n ) + 1 == n );
}

TEST( ULongInt_MillerRabinPrimeTest, small )
{
	RNG rng( 0 );
	EXPECT_FALSE( ULongInt( 1 ).MillerRabinPrimeTest( 5, rng ) );
	EXPECT_FALSE( ULongInt( 4 ).MillerRabinPrimeTest( 5, rng ) );
	EXPECT_FALSE( ULongInt( 6 ).MillerRabinPrimeTest( 5, rng ) );
	EXPECT_TRUE( ULongInt( 2 ).MillerRabinPrimeTest( 5, rng ) );
	EXPECT_TRUE( ULongInt( 3 ).MillerRabinPrimeTest( 5, rng ) );
	EXPECT_TRUE( ULongInt( 5 ).MillerRabinPrimeTest( 5, rng ) );
}
TEST( ULongInt_MillerRabinPrimeTest, size_int )
{
	RNG rng( 0 );
	EXPECT_FALSE( ULongInt( 100000 ).MillerRabinPrimeTest( 5, rng ) );
	EXPECT_FALSE( ULongInt( 252601 ).MillerRabinPrimeTest( 5, rng ) );
	EXPECT_TRUE( ULongInt( 101 ).MillerRabinPrimeTest( 5, rng ) );
	EXPECT_TRUE( ULongInt( 524287 ).MillerRabinPrimeTest( 5, rng ) );
}
TEST( ULongInt_MillerRabinPrimeTest, size_int_2 )
{
	RNG rng( 0 );
	FOR( i, 2, 100 )
	{
		EXPECT_TRUE( ULongInt( i ).MillerRabinPrimeTest( 5, rng ) == Math::MillerRabin( i, 5, rng ) );
	}
}
TEST( ULongInt_MillerRabinPrimeTest, large_yes_1 )
{
	RNG rng( 0 );
	EXPECT_TRUE( ULongInt( "1 23456789 01234619" ).MillerRabinPrimeTest( 5, rng ) );
}
TEST( ULongInt_MillerRabinPrimeTest, large_no_1 )
{
	RNG rng( 0 );
	EXPECT_FALSE( ULongInt( "100 123 234" ).MillerRabinPrimeTest( 5, rng ) );
}
TEST( ULongInt_MillerRabinPrimeTest, large_no_2 )
{
	RNG rng( 0 );
	EXPECT_FALSE( ULongInt( "1 23456789 01234561" ).MillerRabinPrimeTest( 5, rng ) );
}

TEST( ULongInt_MillerRabinPrimeTest, nxt_prime )
{
	RNG rng( 0 );
	ULongInt p( "1234 56789012 34561572" );
	ULongInt nxt( "1234 56789012 34561583" );
	EXPECT_TRUE( nxt.MillerRabinPrimeTest( 10, rng ) );
	while( !p.MillerRabinPrimeTest( 10, rng ) )
	{
		ASSERT_FALSE( p == nxt );
		++p;
	}
	EXPECT_TRUE( p == nxt );
}

TEST( ULongInt, Fermat_little_theorem )
{
	ULongInt p( "123 45678901 23456209" );
	ULongInt a( 101 );
	ULongInt r = a.PowerMod( p - 1, p );
	EXPECT_TRUE( r == 1 );
}