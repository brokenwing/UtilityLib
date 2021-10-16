#include "pch.h"
#include "dynamic_priority_queue.h"

using namespace Util;

TEST( DynamicPriorityQueue, basic_heap )
{
	dynamic_priority_queue<int> q;
	q.resize( 10 );
	q.push( 0,10 );
	q.push( 1,5 );
	q.push( 2,20 );
	EXPECT_FALSE( q.empty() );
	EXPECT_EQ( q.size(), 3 );
	EXPECT_EQ( q.top().idx, 2 );
	q.pop();
	EXPECT_EQ( q.size(), 2 );
	EXPECT_EQ( q.top().idx, 0 );
	q.pop();
	EXPECT_EQ( q.size(), 1 );
	EXPECT_EQ( q.top().idx, 1 );
	q.pop();
	EXPECT_EQ( q.size(), 0 );
	EXPECT_TRUE( q.empty() );
}
TEST( DynamicPriorityQueue, large_heap )
{
	dynamic_priority_queue<int> q;
	std::priority_queue<int> std_q;
	int n = 100;
	RNG rng( 0 );
	std::uniform_int_distribution<int> randw( 1, 100 );

	q.resize( n );
	FOR( i, 0, n )
	{
		auto w = randw( rng );
		q.push( i, w );
		std_q.push( w );
		EXPECT_EQ( q.top().key, std_q.top() );
	}
	while( !q.empty() )
	{
		EXPECT_EQ( q.top().key, std_q.top() );
		q.pop();
		std_q.pop();
	}
}
TEST( DynamicPriorityQueue, update_priority )
{
	dynamic_priority_queue<int> q;
	q.resize( 10 );
	q.push( 0, 10 );
	q.push( 1, 20 );
	q.push( 2, 30 );
	q.update_priority( 0, 40 );
	EXPECT_EQ( q.top().key, 40 );
	q.update_priority( 2, 0 );
	q.pop();
	EXPECT_EQ( q.top().key, 20 );
}
TEST( DynamicPriorityQueue, update_priority_large_case )
{
	dynamic_priority_queue<int> q;
	std::vector<int> std_q;
	int n = 100;
	RNG rng( 0 );
	std::uniform_int_distribution<int> randw( 1, 100 );
	std::uniform_int_distribution<int> randidx( 0, n - 1 );
	auto random_update = [&] ()
	{
		auto idx = randidx( rng );
		auto w = randw( rng );
		q.update_priority( idx, w );
		std_q[idx] = w;
	};
	q.resize( n );
	std_q.resize( n, 0 );
	FOR( i, 0, n )
	{
		auto w = randw( rng );
		q.push( i, w );
		std_q[i] = w;
	}
	FOR( i, 0, n )
		random_update();
	while( !q.empty() )
	{
		auto it = std::max_element( std_q.begin(), std_q.end() );
		EXPECT_EQ( q.top().key, *it );
		q.pop();
		*it = 0;
	}
}
TEST( DynamicPriorityQueue, erase_large_case )
{
	dynamic_priority_queue<int> q;
	std::vector<int> std_q;
	int n = 100;
	RNG rng( 0 );
	std::uniform_int_distribution<int> randw( 1, 100 );
	q.resize( n );
	std_q.resize( n, 0 );
	FOR( i, 0, n )
	{
		auto w = randw( rng );
		q.push( i, w );
		std_q[i] = w;
	}
	std::vector<int> order;
	FOR( i, 0, n )
		order.emplace_back( i );
	std::shuffle( order.begin(), order.end(), rng );
	while( !q.empty() )
	{
		auto it = std::max_element( std_q.begin(), std_q.end() );
		EXPECT_EQ( q.top().key, *it );
		auto idx = order.back();
		q.erase( idx );
		std_q[idx] = 0;
		order.pop_back();
	}
}