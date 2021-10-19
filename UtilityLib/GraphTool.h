#pragma once
#include "Graph.h"
#include "VecUtil.h"

namespace Util::GraphTheory
{
template<graph_type Graph>
LFA::vector<int> find_component( const Graph& g, int start_idx )
{
	LFA::unordered_map<int, int> node_idx2idx;
	LFA::vector<int> idx_list;
	if constexpr( _Is_discrete_idx<Graph> )
	{
		idx_list.reserve( g.size_node() );
		for( auto i = g.begin_node(); i != g.end_node(); ++i )
			idx_list.emplace_back( i->GetIdx() );
		node_idx2idx = VecIndexing( idx_list.begin(), idx_list.end() );
	}
	LFA::vector<bool> visit;
	visit.resize( g.size_node(), false );

	int head = 0;
	LFA::vector<int> retval;
	retval.reserve( g.size_node() );
	retval.emplace_back( start_idx );
	if constexpr( _Is_discrete_idx<Graph> )
		visit[node_idx2idx.find( start_idx )->second] = true;
	else
		visit[start_idx] = true;
	while( head < (int)retval.size() )
	{
		auto st = g.GetNode( retval[head] );
		assert( st );
		assert( st->valid() );
		for( auto e = g.GetNextEdge( *st ); e; e = g.GetNextEdge( *e ) )
		{
			assert( e->valid() );
			int t = e->GetDestination();
			if constexpr( _Is_discrete_idx<Graph> )
				t = node_idx2idx.find( t )->second;
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
}