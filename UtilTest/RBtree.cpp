#include "pch.h"
#include "RBtree.h"
#include <set>
#include <random>

namespace
{
template <typename RBtree>
std::vector<typename RBtree::key_type> toVector( const RBtree& tree )
{
	std::vector<typename RBtree::key_type> ret;
	ret.reserve( tree.size() );
	for( auto i = tree.begin(); i != tree.end(); ++i )
		ret.emplace_back( i->get() );
	return ret;
}
template <typename RBtree>
bool is_same_tree( const RBtree& tree, const std::set<typename RBtree::key_type>& me )
{
	if( tree.size() != me.size() )
		return false;
	auto x = tree.begin();
	auto y = me.begin();
	while( x != tree.end() && y != me.end() )
	{
		if( x->get() != *y )
			return false;
		++x;
		++y;
	}
	if( x != tree.end() || y != me.end() )
		return false;
	return true;
}
}

TEST( RBtree, compile )
{
	Util::RBtree<int> tree;
	tree.size();
	tree.empty();
	tree.check();
	tree.clear();
	tree.begin();
	tree.end();
	tree.find( 0 );
	tree.erase( 0 );
	tree.insert( 0 );
	tree.find_rank( 0 );
	tree.get_rank( tree.begin() );
	tree.lower_bound( 0 );
	const auto& const_tree = tree;
	const_tree.begin();
	const_tree.end();
	const_tree.find( 0 );
	const_tree.find_rank( 0 );
	const_tree.lower_bound( 0 );
	const_tree.print();
}
TEST( RBtree, compile_struct )
{
	struct Node
	{
		int x;
		bool operator <( const Node& other )const
		{
			return x < other.x;
		}
	};
	Util::RBtree<Node> tree;
	tree.size();
	tree.empty();
	tree.check();
	tree.clear();
	tree.begin();
	tree.end();
	tree.find( { 0 } );
	tree.erase( { 0 } );
	tree.insert( { 0 } );
	const auto& const_tree = tree;
	const_tree.begin();
	const_tree.end();
	const_tree.find( { 0 } );
	const_tree.print( [] ( const auto& v )
	{
		return "";
	} );
}

TEST( RBtree, print )
{
	Util::RBtree<int> tree;
	tree.insert( 1 );
	tree.insert( 2 );
	std::cout << tree.print();
}
TEST( RBtree, print_node )
{
	struct Node
	{
		int x;
		bool operator <( const Node& other )const
		{
			return x < other.x;
		}
	};
	Util::RBtree<Node> tree;
	tree.insert( { 1 } );
	tree.insert( { 2 } );
	std::cout << tree.print( [] ( auto p )
	{
		return p.x;
	} );
}

TEST( RBtree, bg_ed )
{
	Util::RBtree<int> tree;
	tree.insert( 1 );
	auto x = tree.begin();
	EXPECT_EQ( x->get(), 1 );
	++x;
	EXPECT_EQ( x, tree.end() );
}
TEST( RBtree, bg_ed_loop )
{
	Util::RBtree<int> tree;
	tree.insert( 1 );
	tree.insert( 3 );
	tree.insert( 2 );
	int k = 1;
	for( auto i = tree.begin(); i != tree.end(); ++i )
		EXPECT_EQ( i->get(), k++ );
}

TEST( RBtree, clear )
{
	Util::RBtree<int> tree;
	tree.insert( 1 );
	ASSERT_EQ( tree.size(), 1 );
	ASSERT_FALSE( tree.empty() );
	tree.clear();
	ASSERT_EQ( tree.size(), 0 );
	ASSERT_TRUE( tree.empty() );
	tree.insert( 2 );
	ASSERT_EQ( tree.size(), 1 );
	ASSERT_FALSE( tree.empty() );
	EXPECT_EQ( tree.begin()->get(), 2 );
}

TEST( RBtree, get_rank )
{
	Util::RBtree<int> tree;
	for( int i = 1; i <= 20; i++ )
		tree.insert( i );
	int rank = 1;
	for( auto i = tree.begin(); i != tree.end(); ++i )
		ASSERT_EQ( tree.get_rank( i ), rank++ );
}
TEST( RBtree, lowerbound )
{
	Util::RBtree<int> tree;
	for( int i = 1; i <= 20; i+=2 )
		tree.insert( i );
	for( int i = 1; i <= 19; i++ )
	{
		auto r = tree.lower_bound( i );
		ASSERT_EQ( r->get(), i + ( i % 2 == 0 ) );
	}
	EXPECT_EQ( tree.lower_bound( 99 ), tree.end() );
}
TEST( RBtree, insert_easy )
{
	Util::RBtree<int> tree;
	tree.insert( 2 );
	tree.insert( 1 );
	tree.insert( 3 );
	auto q = toVector( tree );
	EXPECT_TRUE( tree.check() );
	EXPECT_EQ( q, std::vector<int>( { 1,2,3 } ) );
}
TEST( RBtree, insert_return )
{
	Util::RBtree<int> tree;
	auto r1 = tree.insert( 2 );
	auto r2 = tree.insert( 1 );
	auto r3 = tree.insert( 3 );
	EXPECT_EQ( r1->get(), 2 );
	EXPECT_EQ( r2->get(), 1 );
	EXPECT_EQ( r3->get(), 3 );
	EXPECT_EQ( tree.insert( 1 )->get(), 1 );
	EXPECT_EQ( tree.insert( 2 )->get(), 2 );
	EXPECT_EQ( tree.insert( 3 )->get(), 3 );
}
TEST( RBtree, erase_easy )
{
	Util::RBtree<int> tree;
	tree.insert( 2 );
	tree.insert( 1 );
	tree.insert( 3 );
	tree.erase( 2 );
	auto q = toVector( tree );
	EXPECT_TRUE( tree.check() );
	EXPECT_EQ( q, std::vector<int>( { 1,3 } ) );
}
TEST( RBtree, find_easy )
{
	Util::RBtree<int> tree;
	tree.insert( 2 );
	auto r = tree.find( 2 );
	EXPECT_TRUE( tree.check() );
	ASSERT_NE( r, tree.end() );
	EXPECT_EQ( r->get(), 2 );
}
TEST( RBtree, rank_easy )
{
	Util::RBtree<int> tree;
	tree.insert( 2 );
	auto r = tree.find_rank( 1 );
	EXPECT_TRUE( tree.check() );
	ASSERT_NE( r, tree.end() );
	EXPECT_EQ( r->get(), 2 );
	tree.insert( 20 );
	auto r2 = tree.find_rank( 2 );
	ASSERT_NE( r2, tree.end() );
	EXPECT_EQ( r2->get(), 20 );
}

TEST( RBtree, insert_bug )
{
	Util::RBtree<int> tree;
	tree.insert( 8 );
	tree.insert( 27 );
	tree.insert( 29 );
	tree.insert( 24 );
	tree.insert( 7 );
	tree.insert( 15 );
	EXPECT_TRUE( tree.check() );
}

TEST( RBtree, insert_find_large )
{
	const int n = 300;
	const int l = 0;
	const int r = 35;
	Util::RBtree<int> tree;
	std::set<int> me;
	std::mt19937 rng( 0 );
	std::uniform_int_distribution<int> range( l, r );
	
	for( int i = 0; i < n; i++ )
	{
		const int val = range( rng );
		auto r = tree.insert( val );
		me.insert( val );
		//std::cout << i << ' ' << val << '\n';
		ASSERT_TRUE( tree.check() );
		ASSERT_EQ( r->get(), val );
		ASSERT_TRUE( is_same_tree( tree, me ) );
		{
			const int find_val = range( rng );
			auto x = tree.find( find_val );
			auto y = me.find( find_val );
			ASSERT_EQ( y == me.end(), x == tree.end() );
			if( x != tree.end() )
				ASSERT_EQ( x->get(), find_val );
		}
	}
}

TEST( RBtree, insert_findrank_large )
{
	const int n = 300;
	const int l = 0;
	const int r = 35;
	Util::RBtree<int> tree;
	std::set<int> me;
	std::mt19937 rng( 2 );
	std::uniform_int_distribution<int> range( l, r );
	
	for( int i = 0; i < n; i++ )
	{
		const int val = range( rng );
		auto r = tree.insert( val );
		me.insert( val );
		//std::cout << i << ' ' << val << '\n';
		ASSERT_TRUE( tree.check() );
		ASSERT_EQ( r->get(), val );
		ASSERT_TRUE( is_same_tree( tree, me ) );
		{
			const int findk = std::uniform_int_distribution<int>( 1, (int)tree.size() )( rng );
			auto x = tree.find_rank( findk );
			int ans = -1;
			for( int idx = 0; int v : me )
				if( ++idx == findk )
					ans = v;
			ASSERT_TRUE( ans != -1 );
			ASSERT_TRUE( x != tree.end() );
			ASSERT_EQ( x->get(), ans );
		}
	}
}

TEST( RBtree, erase_large )
{
	const int n = 300;
	const int l = 0;
	const int r = 35;
	Util::RBtree<int> tree;
	std::set<int> me;
	std::mt19937 rng( 1 );
	std::uniform_int_distribution<int> range( l, r );
	
	for( int i = 0; i < n; i++ )
	{
		const int val = range( rng );
		auto r = tree.insert( val );
		me.insert( val );
		//std::cout << i << ' ' << val << '\n';
		ASSERT_TRUE( tree.check() );
		ASSERT_EQ( r->get(), val );
		ASSERT_TRUE( is_same_tree( tree, me ) );
		{
			const int erase_val = range( rng );
			tree.erase( erase_val );
			me.erase( erase_val );
			ASSERT_TRUE( tree.check() );
			ASSERT_TRUE( is_same_tree( tree, me ) );
		}
	}
}
