#pragma once
#include "Graph.h"
#include "VecUtil.h"

namespace Util::GraphTheory
{
template<graph_type Graph>
std::vector<int> find_connected_component( const Graph& g, int start_idx )
{
	GraphNodeIdx2List<Graph> node_idx2idx( g );
	return find_connected_component( g, start_idx, node_idx2idx );
}
template<graph_type Graph>
std::vector<int> find_connected_component( const Graph& g, int start_idx, const GraphNodeIdx2List<Graph>& node_idx2idx )
{
	std::vector<bool> visit;
	visit.resize( g.size_node(), false );

	int head = 0;
	std::vector<int> retval;
	retval.reserve( g.size_node() );
	retval.emplace_back( start_idx );
	visit[node_idx2idx[start_idx]] = true;

	while( head < (int)retval.size() )
	{
		auto st = g.GetNode( retval[head] );
		assert( st );
		assert( st->valid() );
		for( auto e = g.GetNextEdge( *st ); e; e = g.GetNextEdge( *e ) )
		{
			assert( e->valid() );
			const int t = node_idx2idx[e->GetDestination()];
			if( !visit[t] )
			{
				visit[t] = true;
				retval.emplace_back( e->GetDestination() );
			}
		}
		++head;
	}
	return retval;
}

template<graph_type Graph, typename Comparator>
std::vector<int> topological_sort( const Graph& g, const Comparator cmp = std::less<int> )
{
	std::priority_queue<int, cmp> heap;

}
}