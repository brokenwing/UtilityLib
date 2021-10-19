#include "pch.h"
#include "SparseGraph.h"
#include "DenseGraph.h"
#include "Dijkstra.h"
#include "GraphTool.h"

using namespace Util::GraphTheory;

TEST( NodeAndEdge, Compile )
{
	BasicNode node;
	BasicEdge edge;
	WeightedNode<int> wNode;
	WeightedEdge<int> wEdge;
	EXPECT_EQ( node.GetIdx(), -1 );
	EXPECT_EQ( edge.GetIdx(), -1 );
	EXPECT_EQ( edge.GetSource(), -1 );
	EXPECT_EQ( edge.GetDestination(), -1 );

	EXPECT_EQ( wNode.GetIdx(), -1 );
	EXPECT_EQ( wNode.GetWeight(), 0 );
	EXPECT_EQ( wEdge.GetIdx(), -1 );
	EXPECT_EQ( wEdge.GetSource(), -1 );
	EXPECT_EQ( wEdge.GetDestination(), -1 );
	EXPECT_EQ( wEdge.GetWeight(), 0 );
}
TEST( NodeAndEdgeConcept, Compile )
{
	node_type auto nt = BasicNode();
	node_type auto nt2 = WeightedNode<int>();
	node_type auto nt3 = WeightedNode<double>();
	edge_type auto et = BasicEdge();
	edge_type auto et2 = WeightedEdge<int>();
	edge_type auto et3 = WeightedEdge<double>();
}
TEST( GraphConcept, CompileSparseGraph )
{
	graph_type auto gt1 = SparseGraph<BasicNode, BasicEdge>();
	graph_type auto gt2 = SparseGraph<WeightedNode<int>, WeightedEdge<int>>();
	graph_type auto gt3 = SparseGraph<BasicNode, WeightedEdge<int>>();
	graph_type auto gt4 = SparseGraph<WeightedNode<int>, BasicEdge>();
}
TEST( GraphConcept, CompileDenseGraph )
{
	graph_type auto gt1 = DenseGraph<BasicNode, BasicEdge>();
	graph_type auto gt2 = DenseGraph<WeightedNode<int>, WeightedEdge<int>>();
	graph_type auto gt3 = DenseGraph<BasicNode, WeightedEdge<int>>();
	graph_type auto gt4 = DenseGraph<WeightedNode<int>, BasicEdge>();
}
TEST( SparseGraph, basic )
{
	//0->1 0->2 1->2
	graph_type auto g = SparseGraph<BasicNode, BasicEdge>();
	g.reserve( 3, 3 );
	//add node
	EXPECT_EQ( g.AddNode().GetIdx(), 0 );
	EXPECT_EQ( g.AddNode().GetIdx(), 1 );
	EXPECT_EQ( g.AddNode().GetIdx(), 2 );
	//add edge
	auto e1 = g.AddEdge( 0, 1 );
	EXPECT_EQ( e1.GetIdx(), 0 );
	EXPECT_EQ( e1.GetSource(), 0 );
	EXPECT_EQ( e1.GetDestination(), 1 );
	auto e2 = g.AddEdge( 0, 2 );
	EXPECT_EQ( e2.GetIdx(), 1 );
	EXPECT_EQ( e2.GetSource(), 0 );
	EXPECT_EQ( e2.GetDestination(), 2 );
	auto e3 = g.AddEdge( 1, 2 );
	EXPECT_EQ( e3.GetIdx(), 2 );
	EXPECT_EQ( e3.GetSource(), 1 );
	EXPECT_EQ( e3.GetDestination(), 2 );
	//size
	EXPECT_EQ( g.size_node(), 3 );
	EXPECT_EQ( g.size_edge(), 3 );
	//node for
	std::set<int> idx_set;
	for( auto i = g.begin_node(); i != g.end_node(); ++i )
		idx_set.insert( i->GetIdx() );
	EXPECT_EQ( idx_set.size(), 3 );
	EXPECT_TRUE( idx_set.find( 0 ) != idx_set.end() );
	EXPECT_TRUE( idx_set.find( 1 ) != idx_set.end() );
	EXPECT_TRUE( idx_set.find( 2 ) != idx_set.end() );
	//edge for
	idx_set.clear();
	for( auto i = g.begin_edge(); i != g.end_edge(); ++i )
		idx_set.insert( i->GetIdx() );
	EXPECT_EQ( idx_set.size(), 3 );
	EXPECT_TRUE( idx_set.find( 0 ) != idx_set.end() );
	EXPECT_TRUE( idx_set.find( 1 ) != idx_set.end() );
	EXPECT_TRUE( idx_set.find( 2 ) != idx_set.end() );
	//has node
	EXPECT_TRUE( g.HasNode( 0 ) );
	EXPECT_TRUE( g.HasNode( 1 ) );
	EXPECT_TRUE( g.HasNode( 2 ) );
	//get node
	auto p1 = g.GetNode( 0 );
	auto p2 = g.GetNode( 1 );
	auto p3 = g.GetNode( 2 );
	ASSERT_TRUE( p1 && p2 && p3 );
	EXPECT_EQ( p1->GetIdx(), 0 );
	EXPECT_EQ( p2->GetIdx(), 1 );
	EXPECT_EQ( p3->GetIdx(), 2 );
	//next edge
	auto p1_e1 = g.GetNextEdge( *p1 );
	EXPECT_TRUE( p1_e1->GetIdx() == e1.GetIdx() || p1_e1->GetIdx() == e2.GetIdx() );
	auto p1_e2 = g.GetNextEdge( *p1_e1 );
	EXPECT_TRUE( p1_e2->GetIdx() == e1.GetIdx() || p1_e2->GetIdx() == e2.GetIdx() );
	EXPECT_TRUE( g.GetNextEdge( *p1_e2 ) == nullptr );
	EXPECT_EQ( g.GetNextEdge( *p2 )->GetIdx(), e3.GetIdx() );
	EXPECT_EQ( g.GetNextEdge( e3 ), nullptr );
	EXPECT_EQ( g.GetNextEdge( *p3 ), nullptr );
	//out degree
	EXPECT_EQ( g.CountOutDegree( *p1 ), 2 );
	EXPECT_EQ( g.CountOutDegree( *p2 ), 1 );
	EXPECT_EQ( g.CountOutDegree( *p3 ), 0 );
	//inverse edge
	EXPECT_TRUE( g.GetNextInverseEdge( *p1 ) == nullptr );
	EXPECT_EQ( g.GetNextInverseEdge( *p2 )->GetIdx(), e1.GetIdx() );
	EXPECT_EQ( g.GetNextInverseEdge( e1 ), nullptr );
	auto p3_inv_e1 = g.GetNextInverseEdge( *p3 );
	EXPECT_TRUE( p3_inv_e1->GetIdx() == e2.GetIdx() || p3_inv_e1->GetIdx() == e3.GetIdx() );
	auto p3_inv_e2 = g.GetNextInverseEdge( *p3_inv_e1 );
	EXPECT_TRUE( p3_inv_e2->GetIdx() == e2.GetIdx() || p3_inv_e2->GetIdx() == e3.GetIdx() );
	EXPECT_EQ( g.GetNextInverseEdge( *p3_inv_e2 ), nullptr );
	//in degree
	EXPECT_EQ( g.CountInDegree( *p1 ), 0 );
	EXPECT_EQ( g.CountInDegree( *p2 ), 1 );
	EXPECT_EQ( g.CountInDegree( *p3 ), 2 );
	//clear
	g.clear();
	EXPECT_EQ( g.size_node(), 0 );
	EXPECT_EQ( g.size_edge(), 0 );
	EXPECT_EQ( g.begin_node(), g.end_node() );
	EXPECT_EQ( g.begin_edge(), g.end_edge() );
	//resize
	g.resize( 3 );
	EXPECT_EQ( g.size_node(), 3 );
	EXPECT_EQ( g.size_edge(), 0 );
	EXPECT_TRUE( g.HasNode( 0 ) );
	EXPECT_TRUE( g.HasNode( 1 ) );
	EXPECT_TRUE( g.HasNode( 2 ) );
}
TEST( DenseGraph, basic )
{
	graph_type auto g = DenseGraph<BasicNode, BasicEdge>();
	g.resize( 3 );
	//add edge
	auto e1 = g.AddEdge( 0, 1 );
	EXPECT_TRUE( e1.valid() );
	EXPECT_EQ( e1.GetSource(), 0 );
	EXPECT_EQ( e1.GetDestination(), 1 );
	auto e2 = g.AddEdge( 0, 2 );
	EXPECT_TRUE( e2.valid() );
	EXPECT_EQ( e2.GetSource(), 0 );
	EXPECT_EQ( e2.GetDestination(), 2 );
	auto e3 = g.AddEdge( 1, 2 );
	EXPECT_TRUE( e3.valid() );
	EXPECT_EQ( e3.GetSource(), 1 );
	EXPECT_EQ( e3.GetDestination(), 2 );
	//get edge
	EXPECT_EQ( g.GetEdge( 0, 1 )->GetIdx(), e1.GetIdx() );
	EXPECT_EQ( g.GetEdge( 0, 2 )->GetIdx(), e2.GetIdx() );
	EXPECT_EQ( g.GetEdge( 1, 2 )->GetIdx(), e3.GetIdx() );
	//size
	EXPECT_EQ( g.size_node(), 3 );
	EXPECT_EQ( g.size_edge(), 3 );
	//node for
	std::set<int> idx_set;
	for( auto i = g.begin_node(); i != g.end_node(); ++i )
		idx_set.insert( i->GetIdx() );
	EXPECT_EQ( idx_set.size(), 3 );
	EXPECT_TRUE( idx_set.find( 0 ) != idx_set.end() );
	EXPECT_TRUE( idx_set.find( 1 ) != idx_set.end() );
	EXPECT_TRUE( idx_set.find( 2 ) != idx_set.end() );
	//edge for
	idx_set.clear();
	for( auto i = g.begin_edge(); i != g.end_edge(); ++i )
		idx_set.insert( i->GetIdx() );
	EXPECT_EQ( idx_set.size(), 3 );
	EXPECT_TRUE( idx_set.find( e1.GetIdx() ) != idx_set.end() );
	EXPECT_TRUE( idx_set.find( e2.GetIdx() ) != idx_set.end() );
	EXPECT_TRUE( idx_set.find( e3.GetIdx() ) != idx_set.end() );
	//has node
	EXPECT_TRUE( g.HasNode( 0 ) );
	EXPECT_TRUE( g.HasNode( 1 ) );
	EXPECT_TRUE( g.HasNode( 2 ) );
	//get node
	auto p1 = g.GetNode( 0 );
	auto p2 = g.GetNode( 1 );
	auto p3 = g.GetNode( 2 );
	ASSERT_TRUE( p1 && p2 && p3 );
	EXPECT_EQ( p1->GetIdx(), 0 );
	EXPECT_EQ( p2->GetIdx(), 1 );
	EXPECT_EQ( p3->GetIdx(), 2 );
	//next edge
	auto p1_e1 = g.GetNextEdge( *p1 );
	EXPECT_TRUE( p1_e1->GetIdx() == e1.GetIdx() || p1_e1->GetIdx() == e2.GetIdx() );
	auto p1_e2 = g.GetNextEdge( *p1_e1 );
	EXPECT_TRUE( p1_e2->GetIdx() == e1.GetIdx() || p1_e2->GetIdx() == e2.GetIdx() );
	EXPECT_TRUE( g.GetNextEdge( *p1_e2 ) == nullptr );
	EXPECT_EQ( g.GetNextEdge( *p2 )->GetIdx(), e3.GetIdx() );
	EXPECT_EQ( g.GetNextEdge( e3 ), nullptr );
	EXPECT_EQ( g.GetNextEdge( *p3 ), nullptr );
	//out degree
	EXPECT_EQ( g.CountOutDegree( *p1 ), 2 );
	EXPECT_EQ( g.CountOutDegree( *p2 ), 1 );
	EXPECT_EQ( g.CountOutDegree( *p3 ), 0 );
	//inverse edge
	EXPECT_TRUE( g.GetNextInverseEdge( *p1 ) == nullptr );
	EXPECT_EQ( g.GetNextInverseEdge( *p2 )->GetIdx(), e1.GetIdx() );
	EXPECT_EQ( g.GetNextInverseEdge( e1 ), nullptr );
	auto p3_inv_e1 = g.GetNextInverseEdge( *p3 );
	EXPECT_TRUE( p3_inv_e1->GetIdx() == e2.GetIdx() || p3_inv_e1->GetIdx() == e3.GetIdx() );
	auto p3_inv_e2 = g.GetNextInverseEdge( *p3_inv_e1 );
	EXPECT_TRUE( p3_inv_e2->GetIdx() == e2.GetIdx() || p3_inv_e2->GetIdx() == e3.GetIdx() );
	EXPECT_EQ( g.GetNextInverseEdge( *p3_inv_e2 ), nullptr );
	//in degree
	EXPECT_EQ( g.CountInDegree( *p1 ), 0 );
	EXPECT_EQ( g.CountInDegree( *p2 ), 1 );
	EXPECT_EQ( g.CountInDegree( *p3 ), 2 );
	//clear
	g.clear();
	EXPECT_EQ( g.size_node(), 0 );
	EXPECT_EQ( g.size_edge(), 0 );
	EXPECT_EQ( g.begin_node(), g.end_node() );
	EXPECT_EQ( g.begin_edge(), g.end_edge() );
	//resize
	g.resize( 3 );
	EXPECT_EQ( g.size_node(), 3 );
	EXPECT_EQ( g.size_edge(), 0 );
	EXPECT_TRUE( g.HasNode( 0 ) );
	EXPECT_TRUE( g.HasNode( 1 ) );
	EXPECT_TRUE( g.HasNode( 2 ) );
}
TEST( DenseGraph, erase )
{
	graph_type auto g = DenseGraph<BasicNode, BasicEdge>();
	g.resize( 3 );
	auto& e1 = g.AddEdge( 0, 1 );
	auto& e2 = g.AddEdge( 1, 2 );
	auto& e3 = g.AddEdge( 0, 2 );
	g.EraseEdge( e2 );
	EXPECT_EQ( g.size_edge(), 2 );
	EXPECT_TRUE( !g.GetEdge( 1, 2 ) );
	EXPECT_TRUE( e1.valid() );
	EXPECT_TRUE( e3.valid() );
}

TEST( Dijkstra, DenseGraph_easy )
{
	DenseGraph<BasicNode, WeightedEdge<int>> g;
	g.resize( 3 );
	g.AddEdge( 0, 2 ).GetWeight() = 10;
	g.AddEdge( 0, 1 ).GetWeight() = 3;
	g.AddEdge( 1, 2 ).GetWeight() = 3;
	auto r = Dijkstra( g, 0, 2 );
	EXPECT_EQ( r.GetDistance(), 6 );
	EXPECT_TRUE( r.isReachable() );
}
TEST( Dijkstra, DenseGraph_large )
{
	DenseGraph<BasicNode, WeightedEdge<int>> g;
	std::uniform_int_distribution<int> randw( 1, 100 );
	Util::RNG rng( 0 );
	const int n = 100;
	g.resize( n );
	FOR( i, 0, n )
		FOR( j, 0, n )
			if( i != j )
				g.AddEdge( i, j ).GetWeight() = randw( rng );
	FOR( i, 1, n )
		g.GetEdge( i - 1, i )->GetWeight() = 0;
	auto r = Dijkstra( g, 0, n - 1 );
	EXPECT_EQ( r.GetDistance(), 0 );
	EXPECT_TRUE( r.isReachable() );
}
TEST( Dijkstra, SparseGraph )
{
	SparseGraph<BasicNode, WeightedEdge<int>> g;
	g.resize( 3 );
	g.AddEdge( 0, 2 ).GetWeight() = 10;
	g.AddEdge( 0, 1 ).GetWeight() = 3;
	g.AddEdge( 1, 2 ).GetWeight() = 3;
	auto r = Dijkstra( g, 0, 2 );
	EXPECT_EQ( r.GetDistance(), 6 );
	EXPECT_TRUE( r.isReachable() );
}
TEST( Dijkstra, SparseGraph_large )
{
	SparseGraph<BasicNode, WeightedEdge<int>> g;
	std::uniform_int_distribution<int> randw( 1, 100 );
	Util::RNG rng( 0 );
	const int n = 100;
	g.resize( n );
	FOR( i, 0, n )
		FOR( j, 0, n )
			if( i != j )
				g.AddEdge( i, j ).GetWeight() = randw( rng );
	FOR( i, 1, n )
		g.AddEdge( i - 1, i ).GetWeight() = 0;
	auto r = Dijkstra( g, 0, n - 1 );
	EXPECT_EQ( r.GetDistance(), 0 );
	EXPECT_TRUE( r.isReachable() );
}
TEST( Tool, findcompoent )
{
	SparseGraph<> g;
	g.resize( 4 );
	g.AddEdge( 0, 1 );
	g.AddEdge( 2, 3 );
	auto r = find_component( g, 0 );
	ASSERT_EQ( r.size(), 2 );
	EXPECT_TRUE( ( r[0] == 0 && r[1] == 1 ) || ( r[0] == 1 && r[1] == 0 ) );
	auto r2 = find_component( g, 1 );
	ASSERT_EQ( r2.size(), 1 );
	EXPECT_EQ( r2[0], 1 );
}