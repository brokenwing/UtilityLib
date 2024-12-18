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

template<graph_type Graph>
std::vector<int> topological_sort( const Graph& g, const GraphNodeIdx2List<Graph>& node_idx2idx )
{
	std::vector<int> q;//keep node idx
	q.reserve( g.size_node() );

	std::vector<int> indegree;
	indegree.resize( g.size_node() );
	for( auto p = g.begin_node(); p != g.end_node(); ++p )
		indegree[node_idx2idx[p->GetIdx()]] = (int)g.CountInDegree( *p );

	for( int i = 0; i < (int)indegree.size(); ++i )
		if( indegree[i] == 0 )
			q.emplace_back( node_idx2idx.GetNodeIdx( i ) );

	int h = 0;
	while( h < (int)q.size() )
	{
		auto st = g.GetNode( q[h] );
		for( auto e = g.GetNextEdge( *st ); e; e = g.GetNextEdge( *e ) )
		{
			const int nxt_idx = node_idx2idx[e->GetDestination()];
			if( --indegree[nxt_idx] == 0 )
				q.emplace_back( e->GetDestination() );
		}
		++h;
	}
	return q;
}
template<graph_type Graph>
std::vector<int> topological_sort( const Graph& g )
{
	GraphNodeIdx2List<Graph> node_idx2idx( g );
	return topological_sort( g, node_idx2idx );
}
}