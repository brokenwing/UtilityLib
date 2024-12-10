#include "pch.h"
#include "Tarjan.h"
#include <random>

TEST( Tarjan, linear )
{
	Util::Tarjan me;

	me.reset( 3 );
	me.add( 0, 1 );
	me.add( 1, 2 );
	auto ret = me.solve();
	auto sum = me.get_result_group_size();

	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 3 );
	EXPECT_EQ( sum[0], 1 );
	EXPECT_EQ( sum[1], 1 );
	EXPECT_EQ( sum[2], 1 );
}
TEST( Tarjan, no_edge )
{
	Util::Tarjan me;

	me.reset( 3 );
	auto ret = me.solve();
	auto sum = me.get_result_group_size();

	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 3 );
	EXPECT_EQ( sum[0], 1 );
	EXPECT_EQ( sum[1], 1 );
	EXPECT_EQ( sum[2], 1 );
}
//{0},{1,2}
TEST( Tarjan, easy_loop )
{
	Util::Tarjan me;

	me.reset( 3 );
	me.add( 0, 1 );
	me.add( 0, 2 );
	me.add( 1, 2 );
	me.add( 2, 1 );
	auto ret = me.solve();
	auto sum = me.get_result_group_size();
	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 2 );
	EXPECT_EQ( ret[1], ret[2] );
	EXPECT_EQ( sum[0], 1 );
	EXPECT_EQ( sum[ret[1]], 2 );
	EXPECT_EQ( sum[ret[2]], 2 );
}
TEST( Tarjan, dup_edge )
{
	Util::Tarjan me;

	me.reset( 3 );
	me.add( 0, 1 );
	me.add( 0, 1 );
	me.add( 0, 2 );
	me.add( 1, 2 );
	me.add( 1, 2 );
	me.add( 2, 1 );
	me.add( 2, 1 );
	auto ret = me.solve();
	auto sum = me.get_result_group_size();
	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 2 );
	EXPECT_EQ( ret[1], ret[2] );
	EXPECT_EQ( sum[0], 1 );
	EXPECT_EQ( sum[ret[1]], 2 );
	EXPECT_EQ( sum[ret[2]], 2 );
}
TEST( Tarjan, self_loop )
{
	Util::Tarjan me;

	me.reset( 3 );
	me.add( 0, 0 );
	me.add( 1, 1 );
	auto ret = me.solve();
	auto sum = me.get_result_group_size();

	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 3 );
	EXPECT_EQ( sum[0], 1 );
	EXPECT_EQ( sum[1], 1 );
	EXPECT_EQ( sum[2], 1 );
}
//{0,1,2},{5,6,7,8},{3},{4}
TEST( Tarjan, normal_loop )
{
	Util::Tarjan me;

	me.reset( 9 );
	me.add( 0, 1 );
	me.add( 1, 2 );
	me.add( 2, 0 );

	me.add( 2, 3 );
	me.add( 3, 4 );

	me.add( 2, 5 );
	me.add( 5, 6 );
	me.add( 6, 7 );
	me.add( 7, 8 );
	me.add( 8, 5 );

	auto ret = me.solve();
	auto sum = me.get_result_group_size();
	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 4 );
	EXPECT_EQ( ret[1], ret[2] );
	EXPECT_EQ( ret[0], ret[2] );
	
	EXPECT_EQ( ret[5], ret[6] );
	EXPECT_EQ( ret[5], ret[7] );
	EXPECT_EQ( ret[5], ret[8] );

	EXPECT_EQ( sum[ret[0]], 3 );
	EXPECT_EQ( sum[ret[3]], 1 );
	EXPECT_EQ( sum[ret[4]], 1 );
	EXPECT_EQ( sum[ret[5]], 4 );
}

TEST( Tarjan, NonDFS_linear )
{
	Util::Tarjan me;

	me.reset( 3 );
	me.add( 0, 1 );
	me.add( 1, 2 );
	auto ret = me.solve( Util::Tarjan::tMethod::kNonDFS );
	auto sum = me.get_result_group_size();

	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 3 );
	EXPECT_EQ( sum[0], 1 );
	EXPECT_EQ( sum[1], 1 );
	EXPECT_EQ( sum[2], 1 );
}
TEST( Tarjan, NonDFS_no_edge )
{
	Util::Tarjan me;

	me.reset( 3 );
	auto ret = me.solve( Util::Tarjan::tMethod::kNonDFS );
	auto sum = me.get_result_group_size();

	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 3 );
	EXPECT_EQ( sum[0], 1 );
	EXPECT_EQ( sum[1], 1 );
	EXPECT_EQ( sum[2], 1 );
}
//{0},{1,2}
TEST( Tarjan, NonDFS_easy_loop )
{
	Util::Tarjan me;

	me.reset( 3 );
	me.add( 0, 1 );
	me.add( 0, 2 );
	me.add( 1, 2 );
	me.add( 2, 1 );
	auto ret = me.solve( Util::Tarjan::tMethod::kNonDFS );
	auto sum = me.get_result_group_size();
	std::set<int> pt;
	for( int val : ret )
		pt.insert( val );

	EXPECT_EQ( pt.size(), 2 );
	EXPECT_EQ( ret[1], ret[2] );
	EXPECT_EQ( sum[0], 1 );
	EXPECT_EQ( sum[ret[1]], 2 );
	EXPECT_EQ( sum[ret[2]], 2 );
}

TEST( Tarjan, NonDFS_check_by_DFS )
{
	int test_n = 100;
	const int maxn = 50;
	const int minn = 5;

	std::mt19937 rng( 0 );
	while( test_n-- > 0 )
	{
		Util::Tarjan tj_dfs;
		Util::Tarjan tj_ndfs;

		std::uniform_int_distribution<int> randn( minn, maxn );
		const int n = randn( rng );
		std::uniform_int_distribution<int> randm( n, n * n );
		const int m = randm( rng );

		tj_dfs.reset( n );
		tj_ndfs.reset( n );

		std::uniform_int_distribution<int> randpt( 0, n - 1 );
		for( int i = 0; i < m; i++ )
		{
			int p1 = randpt( rng );
			int p2 = randpt( rng );

			tj_dfs.add( p1, p2 );
			tj_ndfs.add( p1, p2 );
		}

		ASSERT_NO_THROW( tj_dfs.solve( Util::Tarjan::tMethod::kDFS ) );
		ASSERT_NO_THROW( tj_ndfs.solve( Util::Tarjan::tMethod::kNonDFS ) );
		auto g1 = tj_dfs.get_result_group();
		auto g2 = tj_ndfs.get_result_group();
		auto s1 = tj_dfs.get_result_group_size();
		auto s2 = tj_ndfs.get_result_group_size();

		ASSERT_EQ( g1.size(), g2.size() );
		ASSERT_EQ( s1.size(), s2.size() );
		bool suc = true;
		for( int i = 0; i < n; i++ )
			for( int j = i + 1; j < n; j++ )
			{
				int x = g1[i] == g1[j];
				int y = g2[i] == g2[j];
				if( x != y )
					suc = false;
				EXPECT_EQ( x, y );//group state equal
			}

		ASSERT_TRUE( suc );
	}
}
