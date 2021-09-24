#include "pch.h"
#include "LongInt.h"

using namespace Util;

TEST( LongInt, postive_string_num )
{
	auto in = "123 456 789";
	auto out = "123 00000456 00000789";
	LongInt a( in, ' ' );
	auto s = a.ToString( ' ' );
	EXPECT_EQ( s, out );
}
TEST( LongInt, neg_string_num )
{
	auto in = "-123 456 789";
	auto out = "-123 00000456 00000789";
	LongInt a( in, ' ' );
	auto s = a.ToString( ' ' );
	EXPECT_EQ( s, out );
}
TEST( LongInt, string_zero )
{
	auto s0 = "0";
	LongInt a( s0, ' ' );
	auto s1 = a.ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, string_one )
{
	auto s0 = "1";
	LongInt a( s0, ' ' );
	auto s1 = a.ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, string_neg_1 )
{
	auto s0 = "-1";
	LongInt a( s0, ' ' );
	auto s1 = a.ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, int_1 )
{
	auto s0 = "1";
	LongInt a( 1 );
	auto s1 = a.ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, neg_int_1 )
{
	auto s0 = "-1";
	LongInt a( -1 );
	auto s1 = a.ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}

TEST( LongInt, add_1 )
{
	auto s0 = "246 00000912";
	LongInt a( "123 456" );
	LongInt b( "123 456" );
	auto s1 = ( a + b ).ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, add_2 )
{
	auto s0 = "247 80000000";
	LongInt a( "123 90000000" );
	LongInt b( "123 90000000" );
	auto s1 = ( a + b ).ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, add_3 )
{
	auto s0 = "100 00000000 00000000";
	LongInt a( "99 99999999 99999999" );
	LongInt b( "1" );
	auto s1 = ( a + b ).ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, add_4 )
{
	auto s0 = "1 00000000 00000000";
	LongInt a( "99999999 99999999" );
	LongInt b( "1" );
	auto s1 = ( a + b ).ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}

TEST( LongInt, minus_1 )
{
	auto s0 = "-1";
	LongInt a( "99999999 99999999" );
	LongInt b( "1 00000000 00000000" );
	auto s1 = ( a - b ).ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, minus_2 )
{
	auto s0 = "1";
	LongInt a( "99999999 99999999" );
	LongInt b( "1 00000000 00000000" );
	auto s1 = ( b - a ).ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, minus_3 )
{
	auto s0 = "1 00000000 00000000";
	LongInt a( s0 );
	LongInt b( s0 );
	auto s1 = ( b - a ).ToString( ' ' );
	EXPECT_EQ( "0", s1 );
}
TEST( LongInt, minus_4 )
{
	auto s0 = "99999999 99999999";
	LongInt a( "1" );
	LongInt b( "1 00000000 00000000" );
	auto s1 = ( b - a ).ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, minus_5 )
{
	auto s0 = "-99999999 99999999";
	LongInt a( "1" );
	LongInt b( "1 00000000 00000000" );
	auto s1 = ( a - b ).ToString( ' ' );
	EXPECT_EQ( s0, s1 );
}
TEST( LongInt, plus_minus_1 )
{
	LongInt a( "123 456 789 321 654 987" );
	LongInt b( "1 00000000 00000000" );
	auto s = ( a + b - a - b ).ToString( ' ' );
	EXPECT_EQ( s, "0" );
}
