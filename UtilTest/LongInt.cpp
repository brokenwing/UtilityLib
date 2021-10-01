#include "pch.h"
#include "LongInt.h"

using namespace Util;

TEST( LongInt, int_construct )
{
	auto out = "10 00000000";
	LongInt a( 1000000000 );//10^9
	auto s = a.ToString( ' ' );
	EXPECT_EQ( s, out );
}
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
TEST( LongInt, mul_small_7 )
{
	LongInt a( "-123 45678909 87654321" );
	auto s = ( a * -1 ).ToString( ' ' );
	EXPECT_EQ( s, "123 45678909 87654321" );
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
TEST( LongInt, mul_large_5 )
{
	LongInt a( "-45678909 87654321" );
	LongInt b( "45678909 87654321" );
	auto s = ( b * a ).ToString();
	EXPECT_EQ( s, "-20865628075093568166437789971041" );
}
TEST( LongInt, mul_large_6 )
{
	LongInt a( "-45678909 87654321" );
	LongInt b( "-45678909 87654321" );
	auto s = ( b * a ).ToString();
	EXPECT_EQ( s, "20865628075093568166437789971041" );
}
TEST( LongInt, mul_large_7 )
{
	LongInt a( "45678909 87654321" );
	LongInt b( "-45678909 87654321" );
	auto s = ( b * a ).ToString();
	EXPECT_EQ( s, "-20865628075093568166437789971041" );
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
TEST( LongInt, div_sign_1 )
{
	LongInt a( "-123 45600088 45600088" );
	LongInt b( "2" );
	auto s = ( a / b ).ToString( ' ' );
	EXPECT_EQ( s, "-61 72800044 22800044" );
}
TEST( LongInt, div_sign_2 )
{
	LongInt a( "-123 45600088 45600088" );
	LongInt b( "-2" );
	auto s = ( a / b ).ToString( ' ' );
	EXPECT_EQ( s, "61 72800044 22800044" );
}
TEST( LongInt, div_sign_3 )
{
	LongInt a( "123 45600088 45600088" );
	LongInt b( "-2" );
	auto s = ( a / b ).ToString( ' ' );
	EXPECT_EQ( s, "-61 72800044 22800044" );
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
TEST( LongInt, mod_sign_1)
{
	LongInt a( "-123 45600000 78900008" );
	LongInt b( "256 98765432" );
	auto s = ( a % b ).ToString( ' ' );
	EXPECT_EQ( s, "-206 07101456" );
}
TEST( LongInt, mod_sign_2)
{
	LongInt a( "123 45600000 78900008" );
	LongInt b( "-256 98765432" );
	auto s = ( a % b ).ToString( ' ' );
	EXPECT_EQ( s, "206 07101456" );
}
TEST( LongInt, mod_sign_3)
{
	LongInt a( "-123 45600000 78900008" );
	LongInt b( "-256 98765432" );
	auto s = ( a % b ).ToString( ' ' );
	EXPECT_EQ( s, "-206 07101456" );
}
TEST( LongInt, exponent_1)
{
	LongInt a( "123 45600000" );
	auto s1 = ( a ^ 2 ).ToString( ' ' );
	auto s2 = ( a * a ).ToString( ' ' );
	EXPECT_EQ( s1, s2 );
}
TEST( LongInt, exponent_2)
{
	LongInt a( "123 45600000" );
	auto s1 = ( a ^ 1 ).ToString( ' ' );
	auto s2 = ( a ).ToString( ' ' );
	EXPECT_EQ( s1, s2 );
}
TEST( LongInt, exponent_3)
{
	LongInt a( "123 45600000" );
	auto s1 = ( a ^ 0 ).ToString( ' ' );
	auto s2 = "1";
	EXPECT_EQ( s1, s2 );
}
TEST( LongInt, exponent_4)
{
	LongInt a( "123 45600000" );
	auto s1 = ( a ^ 5 ).ToString( ' ' );
	auto s2 = ( a * a * a * a * a ).ToString( ' ' );
	EXPECT_EQ( s1, s2 );
}
TEST( LongInt, exponent_5)
{
	LongInt a( "123 45600000" );
	auto s1 = ( ( a ^ 88 ) / ( a ^ 55 ) ).ToString( ' ' );
	auto s2 = ( a ^ ( 88 - 55 ) ).ToString( ' ' );
	EXPECT_EQ( s1, s2 );
}
TEST( LongInt, exponent_sign_1)
{
	LongInt a( "-4" );
	auto s1 = ( a ^ 2 ).ToString( ' ' );
	EXPECT_EQ( s1, "16" );
}
TEST( LongInt, exponent_sign_2)
{
	LongInt a( "-4" );
	auto s1 = ( a ^ 3 ).ToString( ' ' );
	EXPECT_EQ( s1, "-64" );
}
TEST( LongInt, exponent_sign_3)
{
	LongInt a( "-4" );
	auto s1 = ( a ^ 1 ).ToString( ' ' );
	EXPECT_EQ( s1, "-4" );
}
TEST( LongInt, exponent_sign_4)
{
	LongInt a( "-4" );
	auto s1 = ( a ^ 0 ).ToString( ' ' );
	EXPECT_EQ( s1, "1" );
}
TEST( _LongInt, rand)
{
	RNG rng( 0 );
	LongInt v = LongInt::Rand( 3, rng );
	std::cout << v.ToString() << '\n';
}
TEST( LongInt, div_int_1)
{
	LongInt a( "123 45600088 45600088" );
	auto s = ( a / 2 ).ToString( ' ' );
	EXPECT_EQ( s, "61 72800044 22800044" );
}
TEST( LongInt, div_int_2)
{
	LongInt a( "123 45600088 45600088" );
	auto s = ( a / 1 ).ToString( ' ' );
	EXPECT_EQ( s, "123 45600088 45600088" );
}
TEST( LongInt, div_int_3)
{
	LongInt a( "123 45600088 45600088" );
	auto s = ( a / -1 ).ToString( ' ' );
	EXPECT_EQ( s, "-123 45600088 45600088" );
}
TEST( LongInt, div_int_4)
{
	LongInt a( "-123 45600088 45600088" );
	auto s = ( a / 1 ).ToString( ' ' );
	EXPECT_EQ( s, "-123 45600088 45600088" );
}
TEST( LongInt, div_int_5)
{
	LongInt a( "-123 45600088 45600088" );
	auto s = ( a / -1 ).ToString( ' ' );
	EXPECT_EQ( s, "123 45600088 45600088" );
}
TEST( LongInt, mod_int_1)
{
	LongInt a( "123 45600088 45600088" );
	auto s = ( a % 2 ).ToString( ' ' );
	EXPECT_EQ( s, "0" );
}
TEST( LongInt, mod_int_2)
{
	LongInt a( "123 45600088 45600088" );
	auto s = ( a % 1 ).ToString( ' ' );
	EXPECT_EQ( s, "0" );
}
TEST( LongInt, mod_int_3)
{
	LongInt a( "123 45600088 45600081" );
	auto s = ( a % 2 ).ToString( ' ' );
	EXPECT_EQ( s, "1" );
}
TEST( LongInt, mod_int_4)
{
	LongInt a( "-123 45600088 45600081" );
	auto s = ( a % 2 ).ToString( ' ' );
	EXPECT_EQ( s, "-1" );
}
TEST( LongInt, mod_int_5)
{
	LongInt a( "123 45600088 45600081" );
	auto s = ( a % -2 ).ToString( ' ' );
	EXPECT_EQ( s, "1" );
}
TEST( LongInt, mod_int_6)
{
	LongInt a( "-123 45600088 45600081" );
	auto s = ( a % -2 ).ToString( ' ' );
	EXPECT_EQ( s, "-1" );
}

TEST( LongIntCompare, eq1)
{
	LongInt a( "123 456" );
	EXPECT_TRUE( a == a );
}
TEST( LongIntCompare, eq2)
{
	LongInt a( "123 456" );
	LongInt b( "-123 456" );
	EXPECT_FALSE( a == b );
}
TEST( LongIntCompare, eq3)
{
	LongInt a( "-123 456" );
	LongInt b( "123 456" );
	EXPECT_FALSE( a == b );
}
TEST( LongIntCompare, eq4)
{
	LongInt a( "-123 456" );
	EXPECT_TRUE( a == a );
}

TEST( LongIntCompare, neq1)
{
	LongInt a( "123 456" );
	EXPECT_FALSE( a != a );
}
TEST( LongIntCompare, neq2)
{
	LongInt a( "123 456" );
	LongInt b( "-123 456" );
	EXPECT_TRUE( a != b );
}
TEST( LongIntCompare, neq3)
{
	LongInt a( "-123 456" );
	LongInt b( "123 456" );
	EXPECT_TRUE( a != b );
}
TEST( LongIntCompare, neq4)
{
	LongInt a( "-123 456" );
	EXPECT_FALSE( a != a );
}

TEST( LongIntCompare, less1)
{
	LongInt a( "123 456" );
	EXPECT_FALSE( a < a );
}
TEST( LongIntCompare, less2)
{
	LongInt a( "-123 456" );
	EXPECT_FALSE( a < a );
}
TEST( LongIntCompare, less3)
{
	LongInt a( "123 456" );
	LongInt b( "-123 456" );
	EXPECT_FALSE( a < b );
	EXPECT_TRUE( b < a );
}
TEST( LongIntCompare, less4)
{
	LongInt a( "123 456" );
	LongInt b( "123 457" );
	EXPECT_TRUE( a < b );
}
TEST( LongIntCompare, less5)
{
	LongInt a( "-123 456" );
	LongInt b( "-123 457" );
	EXPECT_TRUE( b < a );
}

TEST( LongIntCompare, greater1)
{
	LongInt a( "123 456" );
	EXPECT_FALSE( a > a );
}
TEST( LongIntCompare, greater2)
{
	LongInt a( "-123 456" );
	EXPECT_FALSE( a > a );
}
TEST( LongIntCompare, greater3)
{
	LongInt a( "123 456" );
	LongInt b( "-123 456" );
	EXPECT_TRUE( a > b );
	EXPECT_FALSE( b > a );
}
TEST( LongIntCompare, greater4)
{
	LongInt a( "123 456" );
	LongInt b( "123 457" );
	EXPECT_TRUE( b > a );
}
TEST( LongIntCompare, greater5)
{
	LongInt a( "-123 456" );
	LongInt b( "-123 457" );
	EXPECT_TRUE( a > b );
}

TEST( LongIntCompare, LE1)
{
	LongInt a( "123 456" );
	EXPECT_TRUE( a <= a );
}
TEST( LongIntCompare, LE2)
{
	LongInt a( "-123 456" );
	EXPECT_TRUE( a <= a );
}
TEST( LongIntCompare, LE3)
{
	LongInt a( "123 456" );
	LongInt b( "-123 456" );
	EXPECT_FALSE( a <= b );
	EXPECT_TRUE( b <= a );
}
TEST( LongIntCompare, LE4)
{
	LongInt a( "123 456" );
	LongInt b( "123 457" );
	EXPECT_TRUE( a <= b );
}
TEST( LongIntCompare, LE5)
{
	LongInt a( "-123 456" );
	LongInt b( "-123 457" );
	EXPECT_TRUE( b <= a );
}

TEST( LongIntCompare, GE1)
{
	LongInt a( "123 456" );
	EXPECT_TRUE( a >= a );
}
TEST( LongIntCompare, GE2)
{
	LongInt a( "-123 456" );
	EXPECT_TRUE( a >= a );
}
TEST( LongIntCompare, GE3)
{
	LongInt a( "123 456" );
	LongInt b( "-123 456" );
	EXPECT_TRUE( a >= b );
	EXPECT_FALSE( b >= a );
}
TEST( LongIntCompare, GE4)
{
	LongInt a( "123 456" );
	LongInt b( "123 457" );
	EXPECT_TRUE( b >= a );
}
TEST( LongIntCompare, GE5)
{
	LongInt a( "-123 456" );
	LongInt b( "-123 457" );
	EXPECT_TRUE( a >= b );
}

TEST( LongInt_self_operator, add1)
{
	LongInt a( "123" );
	LongInt b = ++a;
	EXPECT_EQ( a.ToString(), "124" );
	EXPECT_EQ( b.ToString(), "124" );
}
TEST( LongInt_self_operator, add2)
{
	LongInt a( "-123" );
	LongInt b = ++a;
	EXPECT_EQ( a.ToString(), "-122" );
	EXPECT_EQ( b.ToString(), "-122" );
}
TEST( LongInt_self_operator, add3)
{
	LongInt a( "123" );
	LongInt b = a++;
	EXPECT_EQ( a.ToString(), "124" );
	EXPECT_EQ( b.ToString(), "123" );
}
TEST( LongInt_self_operator, add4)
{
	LongInt a( "-123" );
	LongInt b = a++;
	EXPECT_EQ( a.ToString(), "-122" );
	EXPECT_EQ( b.ToString(), "-123" );
}

TEST( LongInt_self_operator, dec1)
{
	LongInt a( "123" );
	LongInt b = --a;
	EXPECT_EQ( a.ToString(), "122" );
	EXPECT_EQ( b.ToString(), "122" );
}
TEST( LongInt_self_operator, dec2)
{
	LongInt a( "-123" );
	LongInt b = --a;
	EXPECT_EQ( a.ToString(), "-124" );
	EXPECT_EQ( b.ToString(), "-124" );
}
TEST( LongInt_self_operator, dec3)
{
	LongInt a( "123" );
	LongInt b = a--;
	EXPECT_EQ( a.ToString(), "122" );
	EXPECT_EQ( b.ToString(), "123" );
}
TEST( LongInt_self_operator, dec4)
{
	LongInt a( "-123" );
	LongInt b = a--;
	EXPECT_EQ( a.ToString(), "-124" );
	EXPECT_EQ( b.ToString(), "-123" );
}

TEST( LongInt_self_operator, add_and_set_1)
{
	LongInt a( "123" );
	a += LongInt( 123 );
	EXPECT_EQ( a.ToString(), "246" );
}
TEST( LongInt_self_operator, dec_and_set_1)
{
	LongInt a( "123" );
	a -= LongInt( 123 );
	EXPECT_EQ( a.ToString(), "0" );
}
TEST( LongInt_self_operator, mul_and_set_1)
{
	LongInt a( "123" );
	a *= LongInt( 123 );
	EXPECT_EQ( a.ToString(), "15129" );
}
TEST( LongInt_self_operator, div_and_set_1)
{
	LongInt a( "123" );
	a /= LongInt( 123 );
	EXPECT_EQ( a.ToString(), "1" );
}
TEST( LongInt_self_operator, mod_and_set_1)
{
	LongInt a( "123" );
	a %= LongInt( 2 );
	EXPECT_EQ( a.ToString(), "1" );
}

TEST( LongIntMathTest, Factorial )
{
	auto r = Math::Factorial<LongInt>( 20 );
	EXPECT_EQ( r.ToString(), "2432902008176640000" );
}
TEST( LongIntMathTest, Factorial_small )
{
	auto r = Math::Factorial<LongInt>( 10 );
	EXPECT_EQ( r.ToString(), "3628800" );
}
TEST( LongIntMathTest, Permutation )
{
	auto r = Math::Permutation<LongInt>( 25, 15);
	EXPECT_EQ( r.ToString(), "4274473667143680000" );
}
TEST( LongIntMathTest, Permutation_small )
{
	auto r = Math::Permutation<LongInt>( 10, 5);
	EXPECT_EQ( r.ToString(), "30240" );
}
TEST( LongIntMathTest, Combination )
{
	auto r = Math::Combination<LongInt>( 25,10 );
	EXPECT_EQ( r.ToString(), "3268760" );
}
TEST( LongIntMathTest, Combination_small )
{
	auto r = Math::Combination<LongInt>( 10,2 );
	EXPECT_EQ( r.ToString(), "45" );
}
TEST( LongIntMathTest, gcd_small )
{
	auto r = Math::GCD<LongInt>( 24, 18 );
	EXPECT_EQ( r.ToString(), "6" );
}
TEST( LongIntMathTest, exgcd_small )
{
	LongInt a = 3;
	LongInt b = 2;
	auto [x, y] = Math::ExtGCD<LongInt>( a, b );
	EXPECT_EQ( a * x + b * y, LongInt( 1 ) );
}
TEST( LongIntMathTest, exgcd_small2 )
{
	LongInt a = 10;
	LongInt b = 24;
	auto [x, y] = Math::ExtGCD<LongInt>( a, b );
	LongInt r = a * x + b * y;
	EXPECT_EQ( r, LongInt( 2 ) );
}
TEST( LongIntMathTest, exgcd_small3 )
{
	LongInt a = 123467;
	LongInt b = 987654;
	auto [x, y] = Math::ExtGCD<LongInt>( a, b );
	LongInt r = a * x + b * y;
	EXPECT_EQ( r, LongInt( 1 ) );
}
TEST( LongIntMathTest, inverse_small )
{
	LongInt a = 3;
	LongInt b = 11;
	auto v = Math::Inverse<LongInt>( a, b );
	EXPECT_EQ( a * v % b, LongInt( 1 ) );
}
TEST( LongIntMathTest, inverse_large )
{
	LongInt a( "12345678 12345678" );
	LongInt b( "12345678 12345679" );
	auto v = Math::Inverse<LongInt>( a, b );
	EXPECT_EQ( a * v % b, LongInt( 1 ) );
}
TEST( LongIntMathTest, inverse_large2 )
{
	LongInt a( "12345678 12345678" );
	LongInt b( "12345678 12345679 123567" );
	auto v = Math::Inverse<LongInt>( a, b );
	EXPECT_EQ( a * v % b, Math::GCD( a, b ) );
}

TEST( LongInt, even )
{
	EXPECT_TRUE( LongInt( 0 ).isEven() );
	EXPECT_TRUE( !LongInt( 1 ).isEven() );
	EXPECT_TRUE( LongInt( 2 ).isEven() );
	EXPECT_TRUE( LongInt( -2 ).isEven() );
	EXPECT_TRUE( LongInt( "123 456 788" ).isEven() );
	EXPECT_TRUE( !LongInt( "123 456 789" ).isEven() );
}
TEST( LongInt, odd )
{
	EXPECT_TRUE( LongInt( 1 ).isOdd() );
	EXPECT_TRUE( LongInt( -1 ).isOdd() );
	EXPECT_TRUE( !LongInt( 0 ).isOdd() );
	EXPECT_TRUE( LongInt( "123 456 999" ).isOdd() );
	EXPECT_TRUE( !LongInt( "123 456 998" ).isOdd() );
}
TEST( LongInt, GetHigh )
{
	EXPECT_EQ( LongInt( 1 ).GetHigh( 1 ), 1 );
	EXPECT_EQ( LongInt( "123 456" ).GetHigh( 1 ), 123 );
	EXPECT_EQ( LongInt( "123 456" ).GetHigh( 2 ), 456 );
}
TEST( LongInt, GetLow )
{
	EXPECT_EQ( LongInt( 1 ).GetLow( 1 ), 1 );
	EXPECT_EQ( LongInt( "123 456" ).GetLow( 1 ), 456 );
	EXPECT_EQ( LongInt( "123 456" ).GetLow( 2 ), 123 );
}
TEST( LongInt, ModFastExponentiation_large_mod )
{
	LongInt a = 10;
	LongInt mod( "99999999 99999999 99999999" );
	auto r = a.ModPow( 15, mod );
}
TEST( LongInt, ModFastExponentiation_small_mod )
{
	LongInt a = 10;
	LongInt mod( "12 99999999" );
	auto r = a.ModPow( 15, mod );
	EXPECT_EQ( r.ToString(), "1000769230" );
}
TEST( LongInt, ModFastExponentiation_mod )
{
	LongInt a = 12345;
	LongInt mod( "123 456" );
	auto r1 = a.ModPow( 55, mod );
	auto r2 = ( a ^ 55 ) % mod;
	EXPECT_EQ( r1, r2 );
}