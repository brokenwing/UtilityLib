#include "pch.h"
#include "Mathematics.h"

using namespace Util::Math;

TEST( Math, modulo )
{
	EXPECT_EQ( modulo( 10, 3, 99999 ), 1000 );
	EXPECT_EQ( modulo( 10, 3, 20 ), 0 );
	EXPECT_EQ( modulo( 10, 3, 499 ), 2 );
	long long v = 1000000000;//10^9
	EXPECT_EQ( modulo( v, 10, 101 ), 100 );//overflow check
}

TEST( Math, mulmod )
{
	EXPECT_EQ( mulmod( 2, 3, 10 ), 6 );
	EXPECT_EQ( mulmod( 10, 20, 2 ), 0 );
	EXPECT_EQ( mulmod( 10, 10, 3 ), 1 );
	long long v = (long long)pow( 10, 12 );
	EXPECT_EQ( mulmod( v, v, v + 1 ), 1 );//overflow check
}

TEST( Math, MillerRabin_basic )
{
	Util::RNG rng( 0 );
	int n = 9;
	EXPECT_FALSE( MillerRabin( -1, n, rng ) );
	EXPECT_FALSE( MillerRabin( 0, n, rng ) );
	EXPECT_FALSE( MillerRabin( 1, n, rng ) );
	EXPECT_TRUE( MillerRabin( 2, n, rng ) );
	rng = Util::RNG( 0 );
	EXPECT_TRUE( MillerRabin( 101, n, rng ) );
	rng = Util::RNG( 0 );
	EXPECT_FALSE( MillerRabin( 100, n, rng ) );
}

TEST( Math, MillerRabin_large )
{
	Util::RNG rng( 0 );
	int n = 9;
	rng = Util::RNG( 0 );
	EXPECT_TRUE( MillerRabin( 524287, n, rng ) );
	rng = Util::RNG( 0 );
	EXPECT_FALSE( MillerRabin( 252601, n, rng ) );
}

TEST( Math, isPrime )
{
	EXPECT_FALSE( isPrime( -1 ) );
	EXPECT_FALSE( isPrime( 0 ) );
	EXPECT_FALSE( isPrime( 1 ) );
	EXPECT_TRUE( isPrime( 2 ) );
	EXPECT_TRUE( isPrime( 101 ) );
	EXPECT_FALSE( isPrime( 100 ) );
	EXPECT_TRUE( isPrime( 524287 ) );
	EXPECT_FALSE( isPrime( 252601 ) );
}

TEST( Math, Normalize_1 )
{
	std::vector<double> q = { 1,2,3 };
	std::vector<double> target = { 1.0 / 6,2.0 / 6,3.0 / 6 };
	Normalize( q.begin(), q.end(), 1 );
	for( int i = 0; i < 3; i++ )
		EXPECT_DOUBLE_EQ( q[i], target[i] );
}

TEST( Math, Normalize_2 )
{
	std::vector<double> q = { 1,2,3 };
	int n = 1 + 4 + 9;
	std::vector<double> target = { 1.0 / n,2.0 / n,3.0 / n };
	Normalize( q.begin(), q.end(), 2 );
	for( int i = 0; i < 3; i++ )
		EXPECT_DOUBLE_EQ( q[i], target[i] );
}

TEST( Math, Normalize_3 )
{
	std::vector<double> q = { 1,2,3 };
	int n = 1 + 8 + 27;
	std::vector<double> target = { 1.0 / n,2.0 / n,3.0 / n };
	Normalize( q.begin(), q.end(), 3 );
	for( int i = 0; i < 3; i++ )
		EXPECT_DOUBLE_EQ( q[i], target[i] );
}

TEST( Math, Mean )
{
	int arr[] = { 1,2,3 };
	EXPECT_DOUBLE_EQ( Mean( arr, arr + 3 ), 2 );
	EXPECT_DOUBLE_EQ( Mean( arr, arr + 1 ), 1 );
	EXPECT_DOUBLE_EQ( Mean( arr, arr + 2 ), 1.5 );
}

TEST( Math, Var )
{
	int arr[] = { 1,2,3 };
	EXPECT_DOUBLE_EQ( Variance( arr, arr + 3 ), 2 );
	EXPECT_DOUBLE_EQ( Variance( arr, arr + 3, 2 ), 2 );
	EXPECT_DOUBLE_EQ( Variance( arr, arr + 3, 0 ), 9 + 4 + 1 );
}

TEST( FastExponentiation, _int_ )
{
	int r = FastExponentiation<int>( 10, 3, [] ( int a, int b )->int
	{
		return a * b;
	} );
	EXPECT_EQ( r, 1000 );
}

TEST( FastExponentiation, _matrix_ )
{
	constexpr int n = 3;
	const int m = 5;
	typedef std::vector<std::vector<int>> Matrix;

	auto resize = [] ( Matrix& mat, int val = 0 )->void
	{
		mat.resize( n );
		for( auto& e : mat )
			e.resize( n, val );
	};
	Matrix arr;
	resize( arr, 0 );
	for( int i = 0; i < n; i++ )
		for( int j = 0; j < n; j++ )
			arr[i][j] = ( i + 1 ) * ( j + 1 ) % 7;

	auto matmul = [&resize] ( const Matrix& a, const Matrix& b )->Matrix
	{
		Matrix c;
		resize( c, 0 );
		for( int i = 0; i < n; i++ )
			for( int j = 0; j < n; j++ )
				for( int k = 0; k < n; k++ )
					c[i][j] += a[i][k] * b[k][j];
		return c;
	};
	auto r1 = FastExponentiation<Matrix>( arr, m, matmul );
	auto r2 = arr;
	for( int i = 0; i < m - 1; i++ )
		r2 = matmul( r2, arr );
	for( int i = 0; i < n; i++ )
		for( int j = 0; j < n; j++ )
			EXPECT_EQ( r1[i][j], r2[i][j] );
}

TEST( Math, gcd )
{
	EXPECT_EQ( GCD( 4, 6 ), 2 );
	EXPECT_EQ( GCD( 1, 0 ), 1 );
	EXPECT_EQ( GCD( 0, 1 ), 1 );
	EXPECT_EQ( GCD( 3, 3 ), 3 );
	EXPECT_EQ( GCD( 10, 11 ), 1 );
	EXPECT_EQ( GCD( 17, 101 ), 1 );
}

TEST( Math, extgcd_1 )
{
	int a = 3;
	int b = 2;
	auto [x, y] = ExtGCD( a, b );
	EXPECT_EQ( a * x + b * y, 1 );
}

TEST( Math, extgcd_2 )
{
	int a = 123467;
	int b = 987654;
	auto [x, y] = ExtGCD( a, b );
	EXPECT_EQ( a * x + b * y, 1 );
}

TEST( Math, extgcd_3 )
{
	int a = 10;
	int b = 24;
	auto [x, y] = ExtGCD( a, b );
	EXPECT_EQ( a * x + b * y, 2 );
}