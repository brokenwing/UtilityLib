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
	friend std::vector<DistanceInfo_Dijkstra<Graph>> Dijkstra<Graph>( const Graph&, int, const GraphNodeIdx2List<Graph>& );
	DistanceInfo_Dijkstra()	{}
	const weight_type& GetDistance()const noexcept	{		return dis;	}
	bool isReachable()const noexcept				{		return reachable;	}
};

//Sparse Graph (heap) O((N+E)*log(N))
template <graph_type Graph>
requires weighted_edge_type<typename Graph::Edge>&& _Is_sparse_graph<Graph>
std::vector<DistanceInfo_Dijkstra<Graph>> Dijkstra( const Graph& g, int start_idx, const GraphNodeIdx2List<Graph>& node_idx2idx )
{
	using weight_type = typename Graph::Edge::weight_type;
	std::vector<DistanceInfo_Dijkstra<Graph>> dis_vector;
	const int n = (int)g.size_node();
	dis_vector.resize( n );

	dis_vector[node_idx2idx[start_idx]].reachable = true;
	dynamic_priority_queue<weight_type, std::greater<>> heap;//smallest top
	heap.resize( n );
	int heap_st_idx = node_idx2idx[start_idx];
	heap.push( heap_st_idx, weight_type() );
	
	while( !heap.empty() )
	{
		const int node_idx = node_idx2idx.GetNodeIdx( heap.top().idx );
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
				const int idx = node_idx2idx[e->GetDestination()];
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
std::vector<DistanceInfo_Dijkstra<Graph>> Dijkstra( const Graph& g, int start_idx, const GraphNodeIdx2List<Graph>& node_idx2idx )
{
	std::vector<DistanceInfo_Dijkstra<Graph>> dis_vector;
	const int n = (int)g.size_node();
	dis_vector.resize( n );

	dis_vector[node_idx2idx[start_idx]].reachable = true;
	while( true )
	{
		//find min
		DistanceInfo_Dijkstra<Graph>* best = nullptr;
		int best_idx = -1;
		int cur_idx = 0;
		for( auto& p : dis_vector )
		{
			if( p.reachable && !p.visited && ( !best || best->dis > p.dis ) )
			{
				best = &p;
				best_idx = cur_idx;
			}
			++cur_idx;
		}
		if( !best )
			break;
		//update
		best->visited = true;
		const int best_node_idx = node_idx2idx.GetNodeIdx( best_idx );
		for( int i = 0; i < n; i++ )
		{
			auto edge = g.GetEdge( best_node_idx, i );
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
	GraphNodeIdx2List<Graph> node_idx2idx( g );
	return Dijkstra( g, start_idx, node_idx2idx )[end_idx];
}

template <graph_type Graph>
requires weighted_edge_type<typename Graph::Edge>
std::vector<DistanceInfo_Dijkstra<Graph>> Dijkstra( const Graph& g, int start_idx )
{
	GraphNodeIdx2List<Graph> node_idx2idx( g );
	return Dijkstra( g, start_idx, node_idx2idx );
}
}