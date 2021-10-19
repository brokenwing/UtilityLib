#pragma once
#include "Graph.h"
#include "dynamic_priority_queue.h"
#include "VecUtil.h"

namespace Util::GraphTheory
{
// **Important** Dijkstra only works on *positive* weight 

//Result Info
template <graph_type Graph>
requires weighted_edge_type<typename Graph::Edge>
class DistanceInfo_Dijkstra
{
public:
	using weight_type = typename Graph::Edge::weight_type;
protected:
	weight_type dis = weight_type();
	bool reachable = false;
	bool visited = false;//for Dijkstra
public:
	friend DistanceVector<Graph, DistanceInfo_Dijkstra<Graph>> Dijkstra<Graph>( const Graph&, int );
	DistanceInfo_Dijkstra()	{}
	const weight_type& GetDistance()const noexcept	{		return dis;	}
	bool isReachable()const noexcept				{		return reachable;	}
};

//Sparse Graph (heap) O((N+E)*log(N))
template <graph_type Graph>
requires weighted_edge_type<typename Graph::Edge>&& _Is_sparse_graph<Graph>
DistanceVector<Graph, DistanceInfo_Dijkstra<Graph>> Dijkstra( const Graph& g, int start_idx )
{
	using weight_type = typename Graph::Edge::weight_type;
	DistanceVector<Graph, DistanceInfo_Dijkstra<Graph>> dis_vector;
	const int n = (int)g.size_node();
	if constexpr( _Is_discrete_idx<Graph> )
		dis_vector.reserve( n );
	else
		dis_vector.resize( n );

	//heap use index 0,1,2,...;  node might use any index
	LFA::unordered_map<int, int> node_idx2idx;
	LFA::vector<int> idx_list;
	if constexpr( _Is_discrete_idx<Graph> )
	{
		idx_list.reserve( g.size_node() );
		for( auto i = g.begin_node(); i != g.end_node(); ++i )
			idx_list.emplace_back( i->GetIdx() );
		node_idx2idx = VecIndexing( idx_list.begin(), idx_list.end() );
	}

	dis_vector[start_idx].reachable = true;
	dynamic_priority_queue<weight_type, std::greater<>> heap;//smallest top
	heap.resize( n );
	int heap_st_idx;
	if constexpr( _Is_discrete_idx<Graph> )
		heap_st_idx = node_idx2idx.find( start_idx )->second;
	else
		heap_st_idx = start_idx;
	heap.push( heap_st_idx, weight_type() );
	
	while( !heap.empty() )
	{
		int node_idx;
		if constexpr( _Is_discrete_idx<Graph> )
			node_idx = idx_list[heap.top().idx];
		else
			node_idx = heap.top().idx;
		auto st = g.GetNode( node_idx );
		assert( st );
		assert( st->valid() );
		dis_vector[st->GetIdx()].visited = true;
		for( auto e = g.GetNextEdge( *st ); e; e = g.GetNextEdge( *e ) )
		{
			assert( e->GetSource() == st->GetIdx() );
			assert( e->valid() );
			auto& dest = dis_vector[e->GetDestination()];
			if( !dest.visited && ( !dest.reachable || heap.top().key + e->GetWeight() < dest.dis ) )
			{
				dest.dis = heap.top().key + e->GetWeight();
				int idx;
				if constexpr( _Is_discrete_idx<Graph> )
					idx = node_idx2idx.find( e->GetDestination() )->second;
				else
					idx = e->GetDestination();
				if( dest.reachable )
				{
					assert( heap.exists( idx ) );
					heap.update_priority( idx, dest.dis );
				}
				else
				{
					assert( !heap.exists( idx ) );
					dest.reachable = true;
					heap.push( idx, dest.dis );
				}
			}
		}
		heap.pop();
	}
	return dis_vector;
}

//Dense Graph (brute) O(N^2)
template <graph_type Graph>
requires weighted_edge_type<typename Graph::Edge> && _Is_dense_graph<Graph>
DistanceVector<Graph, DistanceInfo_Dijkstra<Graph>> Dijkstra( const Graph& g, int start_idx )
{
	DistanceVector<Graph, DistanceInfo_Dijkstra<Graph>> dis_vector;
	const int n = (int)g.size_node();
	if constexpr( _Is_discrete_idx<Graph> )
		dis_vector.reserve( n );
	else
		dis_vector.resize( n );

	dis_vector[start_idx].reachable = true;
	while( true )
	{
		//find min
		DistanceInfo_Dijkstra<Graph>* best = nullptr;
		int idx = -1;
		int cur_idx = 0;
		for( auto& p : dis_vector )
		{
			DistanceInfo_Dijkstra<Graph>* ptr = nullptr;
			if constexpr( _Is_discrete_idx<Graph> )
				ptr = &p.second;
			else
				ptr = &p;
			if( ptr->reachable && !ptr->visited && ( !best || best->dis > ptr->dis ) )
			{
				best = ptr;
				if constexpr( _Is_discrete_idx<Graph> )
					idx = p.first;
				else
					idx = cur_idx;
			}
			++cur_idx;
		}
		if( !best )
			break;
		//update
		best->visited = true;
		for( int i = 0; i < n; i++ )
		{
			auto edge = g.GetEdge( idx, i );
			if( !edge )
				continue;
			auto& dest = dis_vector[edge->GetDestination()];
			if( !dest.visited && ( !dest.reachable || best->dis + edge->GetWeight() < dest.dis ) )
			{
				dest.dis = best->dis + edge->GetWeight();
				dest.reachable = true;
			}
		}
	}
	return dis_vector;
}
//Distance from start to end
template <graph_type Graph>
requires weighted_edge_type<typename Graph::Edge>
DistanceInfo_Dijkstra<Graph> Dijkstra( const Graph& g, int start_idx, int end_idx )
{
	return Dijkstra( g, start_idx )[end_idx];
}
}