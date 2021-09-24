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
TEST( LongInt, large_int )
{
	auto s0 = "1 00000001";
	LongInt a( 100000001 );
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
TEST( LongInt, mul_small_1 )
{
	LongInt a( "10" );
	auto s = ( a * 1 ).ToString( ' ' );
	EXPECT_EQ( s, "10" );
}
TEST( LongInt, mul_small_2 )
{
	LongInt a( "10" );
	auto s = ( a * 0 ).ToString( ' ' );
	EXPECT_EQ( s, "0" );
}
TEST( LongInt, mul_small_3 )
{
	LongInt a( "11" );
	auto s = ( a * 9 ).ToString( ' ' );
	EXPECT_EQ( s, "99" );
}
TEST( LongInt, mul_small_4 )
{
	LongInt a( "123 45678909 87654321" );
	auto s = ( a * 9999 ).ToString();
	EXPECT_EQ( s, "12344444341985555555679" );
}
TEST( LongInt, mul_small_5 )
{
	LongInt a( "123 45678909 87654321" );
	auto s = ( a * -1 ).ToString( ' ' );
	EXPECT_EQ( s, "-123 45678909 87654321" );
}
TEST( LongInt, mul_small_6 )
{
	LongInt a( "123 45678909 87654321" );
	auto s = ( a * 0 ).ToString();
	EXPECT_EQ( s, "0" );
}
TEST( LongInt, mul_large_1 )
{
	LongInt a( "1 0" );
	LongInt b( "1 0" );
	auto s = ( a * b ).ToString( ' ' );
	EXPECT_EQ( s, "1 00000000 00000000" );
}
TEST( LongInt, mul_large_2 )
{
	LongInt a( "123 45678909 87654321" );
	LongInt b( "1 0 0" );
	auto s = ( a * b ).ToString( ' ' );
	EXPECT_EQ( s, "123 45678909 87654321 00000000 00000000" );
}
TEST( LongInt, mul_large_3 )
{
	LongInt a( "123 45678909 87654321" );
	LongInt b( "1 0 0" );
	auto s = ( b * a ).ToString( ' ' );
	EXPECT_EQ( s, "123 45678909 87654321 00000000 00000000" );
}
TEST( LongInt, mul_large_4 )
{
	LongInt a( "45678909 87654321" );
	LongInt b( "45678909 87654321" );
	auto s = ( b * a ).ToString();
	EXPECT_EQ( s, "20865628075093568166437789971041" );
}
TEST( LongInt, div_small_1 )
{
	LongInt a( "123" );
	LongInt b( "123" );
	auto s = ( a / b ).ToString();
	EXPECT_EQ( s, "1" );
}
TEST( LongInt, div_small_2 )
{
	LongInt a( "123" );
	LongInt b( "2" );
	auto s = ( a / b ).ToString();
	EXPECT_EQ( s, "61" );
}
TEST( LongInt, div_small_3 )
{
	LongInt a( "0" );
	LongInt b( "123 345 123 7" );
	auto s = ( a / b ).ToString();
	EXPECT_EQ( s, "0" );
}
TEST( LongInt, div_small_4 )
{
	LongInt a( "123 00000088" );
	LongInt b( "1" );
	auto s = ( a / b ).ToString( ' ' );
	EXPECT_EQ( s, "123 00000088" );
}
TEST( LongInt, div_small_5 )
{
	LongInt a( "123 00000088" );
	LongInt b( "13 123 98" );
	auto s = ( a / b ).ToString( ' ' );
	EXPECT_EQ( s, "0" );
}
TEST( LongInt, div_small_6 )
{
	LongInt a( "123 45600088 45600088" );
	LongInt b( "88 12345678" );
	auto s = ( a / b ).ToString( ' ' );
	EXPECT_EQ( s, "1 40094369" );
}
TEST( LongInt, div_small_7 )
{
	LongInt a( "123 45600088 45600088" );
	LongInt b( "2" );
	auto s = ( a / b ).ToString( ' ' );
	EXPECT_EQ( s, "61 72800044 22800044" );
}
TEST( LongInt, mod_1)
{
	LongInt a( "3" );
	LongInt b( "2" );
	auto s = ( a % b ).ToString( ' ' );
	EXPECT_EQ( s, "1" );
}
TEST( LongInt, mod_2)
{
	LongInt a( "0" );
	LongInt b( "2123 13131" );
	auto s = ( a % b ).ToString( ' ' );
	EXPECT_EQ( s, "0" );
}
TEST( LongInt, mod_3)
{
	LongInt a( "312 98123 12397 1235" );
	LongInt b( "1" );
	auto s = ( a % b ).ToString( ' ' );
	EXPECT_EQ( s, "0" );
}
TEST( LongInt, mod_4)
{
	LongInt a( "123 45600000 78900008" );
	LongInt b( "256 98765432" );
	auto s = ( a % b ).ToString( ' ' );
	EXPECT_EQ( s, "206 07101456" );
}
