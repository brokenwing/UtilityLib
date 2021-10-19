#include "pch.h"
#include "Util.h"
#include "CommonDef.h"
#include "Timer.h"
#include <thread>

using namespace Util;

constexpr double EPS_UP = 1.1;
constexpr double EPS_LOW = 0.9;

TEST( Timer, normal_clock )
{
	int dur = 10;
	int err = dur * 2;
	Timer t;
	t.SetTime();
	std::this_thread::sleep_for( std::chrono::milliseconds( dur ) );
	double s = t.GetTime();
	std::int64_t mill = t.GetMilliseconds();
	std::int64_t micro = t.GetMicroseconds();
	std::int64_t nano = t.GetNanoseconds();
	EXPECT_LE( abs( mill - dur ), err );
	EXPECT_LE( abs( micro / 1000 - dur ), err );
	EXPECT_LE( abs( nano / 1000000 - dur ), err );
	EXPECT_LE( abs( (int)( s * 1000 ) - dur ), err );
}
TEST( Timer, high_resolution )
{
	int dur = 10;
	int err = dur * 2;
	HighResolutionTimer t;
	t.SetTime();
	std::this_thread::sleep_for( std::chrono::milliseconds( dur ) );
	double s = t.GetTime();
	std::int64_t mill = t.GetMilliseconds();
	std::int64_t micro = t.GetMicroseconds();
	std::int64_t nano = t.GetNanoseconds();
	EXPECT_LE( abs( mill - dur ), err );
	EXPECT_LE( abs( micro / 1000 - dur ), err );
	EXPECT_LE( abs( nano / 1000000 - dur ), err );
	EXPECT_LE( abs( (int)( s * 1000 ) - dur ), err );
}

TEST( Util, IsZero )
{
	EXPECT_TRUE( IsZero( 0 ) );
	EXPECT_TRUE( IsZero( eps * EPS_LOW ) );
	EXPECT_TRUE( !IsZero( eps * EPS_UP ) );
}
TEST( Util, EQ )
{
	EXPECT_TRUE( EQ( 0, 0 ) );
	EXPECT_TRUE( EQ( 0, eps * EPS_LOW ) );
	EXPECT_TRUE( !EQ( 0, eps * EPS_UP ) );
}
TEST( Util, NEQ )
{
	EXPECT_TRUE( NEQ( 0, 1 ) );
	EXPECT_TRUE( NEQ( 0, eps * EPS_UP ) );
	EXPECT_TRUE( !NEQ( 0, eps * EPS_LOW ) );
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
TEST( Util_Move, R_0_4 )
{
	const int target[5] = { 2,3,4,5,1 };
	int arr[5] = { 1,2,3,4,5 };
	Move( arr + 0, arr + 4 );
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
TEST( Util_Move, L_4_0 )
{
	const int target[5] = { 5,1,2,3,4 };
	int arr[5] = { 1,2,3,4,5 };
	Move( arr + 4, arr + 0 );
	for( int i = 0; i < 5; i++ )
		EXPECT_EQ( arr[i], target[i] );
}

TEST( Util_MoveN, R_1_5_3 )
{
	const int target[5] = { 1,5,2,3,4 };
	int arr[5] = { 1,2,3,4,5 };
	int x = 1;
	int y = 5;
	int n = 3;
	Move( arr + x, arr + y, n );
	for( int i = 0; i < 5; i++ )
		EXPECT_EQ( arr[i], target[i] );
}

TEST( Util_MoveN, R_0_1_3 )
{
	const int target[5] = { 1,2,3,4,5 };
	int arr[5] = { 1,2,3,4,5 };
	int x = 0;
	int y = 1;
	int n = 3;
	Move( arr + x, arr + y, n );
	for( int i = 0; i < 5; i++ )
		EXPECT_EQ( arr[i], target[i] );
}

TEST( Util_MoveN, L_2_1_2 )
{
	const int target[5] = { 1,3,4,2,5 };
	int arr[5] = { 1,2,3,4,5 };
	int x = 2;
	int y = 1;
	int n = 2;
	Move( arr + x, arr + y, n );
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

TEST( OrderedHash, int_rule_test )
{
	EXPECT_EQ( OrderedHash( 1, 2 ), OrderedHash( 1, 2 ) );
	EXPECT_NE( OrderedHash( 1, 2 ), OrderedHash( 2, 1 ) );
	EXPECT_NE( OrderedHash( 1, 1 ), OrderedHash( 2, 2 ) );
}

TEST( OrderedHash, int_small_conflict_test )
{
	std::set<size_t> count;
	int n = 10;
	int m = 10;
	int tot = 0;
	for( int i = -n; i <= n; i++ )
		for( int j = -m; j <= m; j++ )
		{
			count.insert( OrderedHash( i, j ) );
			++tot;
		}
	EXPECT_EQ( count.size(), tot );
}

TEST( OrderedHash, int_random_conflict_test )
{
	std::set<std::pair<int, int>> count;
	std::set<size_t> hash_count;
	RNG rng( 0 );
	int n = 1000;
	for( int i = 0; i < n; i++ )
	{
		auto e = std::make_pair( rng(), rng() );
		count.insert( e );
		hash_count.insert( OrderedHash( e.first, e.second ) );
	}
	EXPECT_EQ( count.size(), hash_count.size() );
}

TEST( UnorderedHash, int_rule_test )
{
	EXPECT_EQ( UnorderedHash( 1, 2 ), UnorderedHash( 1, 2 ) );
	EXPECT_NE( UnorderedHash( 1, 1 ), UnorderedHash( 2, 2 ) );
	//swap eq
	for( int i = 0; i < 10; i++ )
		for( int j = i + 1; j < 10; j++ )
			EXPECT_EQ( UnorderedHash( i, j ), UnorderedHash( j, i ) );
}

TEST( UnorderedHash, int_small_conflict_test )
{
	std::set<size_t> count;
	int n = 10;
	int m = 10;
	int tot = 0;
	for( int i = -n; i <= n; i++ )
		for( int j = i; j <= m; j++ )
		{
			count.insert( UnorderedHash( i, j ) );
			//std::cout << i << ' ' << j << ' ' << UnorderedHash( i, j ) << '\n';
			++tot;
		}
	EXPECT_EQ( count.size(), tot );
}

TEST( UnorderedHash, int_random_conflict_test )
{
	std::set<std::pair<int, int>> count;
	std::set<size_t> hash_count;
	RNG rng( 0 );
	int n = 1000;
	for( int i = 0; i < n; i++ )
	{
		auto e = std::make_pair( rng(), rng() );
		count.insert( e );
		hash_count.insert( UnorderedHash( e.first, e.second ) );
	}
	EXPECT_EQ( count.size(), hash_count.size() );
}

TEST( OrderedPairHash, is_equal_to_OrderedHash )
{
	OrderedPairHash<int, int> h;
	int n = 10;
	int m = 10;
	for( int i = -n; i <= n; i++ )
		for( int j = -m; j <= m; j++ )
		{
			EXPECT_EQ( OrderedHash( i, j ), h( { i,j } ) );
		}
}

TEST( OrderedPairHash, unordered_map )
{
	std::unordered_map<std::pair<int, int>, int, OrderedPairHash<int, int>> map;
	map[{1, 2}] = 3;
	map[{2, 3}] = 4;
	map[{1, 2}]++;
	auto r1 = map[{1, 2}];
	auto r2 = map[{2, 3}];
	EXPECT_EQ( r1, 4 );
	EXPECT_EQ( r2, 4 );
}

TEST( UnorderedPairHash, is_equal_to_UnorderedHash )
{
	UnorderedPairHash<int, int> h;
	int n = 10;
	int m = 10;
	for( int i = -n; i <= n; i++ )
		for( int j = -m; j <= m; j++ )
		{
			EXPECT_EQ( UnorderedHash( i, j ), h( { i,j } ) );
		}
}

TEST( UnorderedPairHash, unordered_map )
{
	std::unordered_map<std::pair<int, int>, int, UnorderedPairHash<int, int>> map;
	map[{1, 2}] = 1;
}

TEST( TupleHash, check_rule_1 )
{
	std::tuple<int> v = { 1 };
	TupleHash<decltype( v )> hash;
	auto r1 = hash( v );
	EXPECT_EQ( r1, std::hash<int>()( 1 ) );
}

TEST( TupleHash, check_stable )
{
	std::tuple<int,int,int> v = { 1,2,3 };
	TupleHash<decltype( v )> hash;
	EXPECT_EQ( hash( v ), hash( v ) );
}

TEST( TupleHash, check_rule_swap )
{
	TupleHash<std::tuple<int, int>> hash;
	int n = 10;
	for( int i = 0; i < n; i++ )
		for( int j = i + 1; j < n; j++ )
		{
			auto r1 = hash( { i,j } );
			auto r2 = hash( { j,i } );
			EXPECT_NE( r1, r2 );
		}
}

TEST( TupleHash, int_small_conflict_test_3d )
{
	std::tuple<int, int, int> v;
	TupleHash<decltype( v )> hash;
	std::set<size_t> count;
	int n = 5;
	for( int i = 0; i < n; i++ )
		for( int j = 0; j < n; j++ )
			for( int k = 0; k < n; k++ )
			{
				v = { i,j,k };
				count.insert( hash( v ) );
			}
	EXPECT_EQ( count.size(), n * n * n );
}

TEST( TupleHash, int_small_conflict_test_4d )
{
	std::tuple<int, int, int, int> v;
	TupleHash<decltype( v )> hash;
	std::set<size_t> count;
	int n = 5;
	for( int i = 0; i < n; i++ )
		for( int j = 0; j < n; j++ )
			for( int k = 0; k < n; k++ )
				for( int l = 0; l < n; l++ )
				{
					v = { i,j,k,l };
					count.insert( hash( v ) );
				}
	EXPECT_EQ( count.size(), n * n * n * n );
}

TEST( TupleHash, int_random_conflict_test )
{
	std::tuple<int, int, int> v;
	TupleHash<decltype( v )> hash;
	std::set<size_t> hash_count;
	std::set<decltype( v )> count;
	RNG rng( 0 );
	int n = 1000;
	for( int i = 0; i < n; i++ )
	{
		v = { rng(),rng(),rng() };
		hash_count.insert( hash( v ) );
		count.insert( v );
	}
	EXPECT_EQ( count.size(), hash_count.size() );
}

TEST( TupleHash, diff_type )
{
	std::tuple<int, double, long long> v = { 1,2.3,123456789000 };
	EXPECT_NO_THROW( TupleHash<decltype( v )>()( v ) );
}

TEST( TupleHash, unordered_map )
{
	typedef std::tuple<int, int, int> tup;
	std::unordered_map<tup, int, TupleHash<tup>> map;
	map[{1, 2, 3}] = 1;
}

TEST( Define, FOR_1 )
{
	std::vector<int> q;
	FOR( i, 0, 3 )
		q.emplace_back( i );
	ASSERT_EQ( q.size(), 3 );
	EXPECT_EQ( q[0], 0 );
	EXPECT_EQ( q[1], 1 );
	EXPECT_EQ( q[2], 2 );
}
TEST( Define, FOR_2 )
{
	std::vector<int> q;
	FOR( i, -3, 3 )
		q.emplace_back( i );
	ASSERT_EQ( q.size(), 6 );
	EXPECT_EQ( q[0], -3 );
	EXPECT_EQ( q[1], -2 );
	EXPECT_EQ( q[2], -1 );
	EXPECT_EQ( q[3], 0 );
	EXPECT_EQ( q[4], 1 );
	EXPECT_EQ( q[5], 2 );
}
