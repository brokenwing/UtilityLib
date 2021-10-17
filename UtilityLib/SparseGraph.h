#pragma once
#include "Graph.h"

namespace Util::GraphTheory
{
template <node_type _Node, edge_type _Edge>
class SparseGraph
{
public:
	class Node;
	class Edge;
	using is_sparse_graph		= std::true_type;
	using is_discrete_idx		= std::false_type;
	using is_node_addable		= std::true_type;
	using is_node_erasable		= std::false_type;
	using is_edge_erasable		= std::false_type;
	using is_edge_duplicatable	= std::true_type;
	using has_inverse_edge		= std::true_type;

protected:
	LFA::vector<Node> m_nodeList;
	LFA::vector<Edge> m_edgeList;

public:
	using iterator_node = typename decltype( m_nodeList )::iterator;
	using iterator_edge = typename decltype( m_edgeList )::iterator;
	using const_iterator_node = typename decltype( m_nodeList )::const_iterator;
	using const_iterator_edge = typename decltype( m_edgeList )::const_iterator;

	SparseGraph(){}
	~SparseGraph(){}
	//Common

	iterator_node begin_node()				{		return m_nodeList.begin();	}
	iterator_node end_node()				{		return m_nodeList.end();	}
	const_iterator_node begin_node()const	{		return m_nodeList.begin();	}
	const_iterator_node end_node()const		{		return m_nodeList.end();	}
	iterator_edge begin_edge()				{		return m_edgeList.begin();	}
	iterator_edge end_edge()				{		return m_edgeList.end();	}
	const_iterator_edge begin_edge()const	{		return m_edgeList.begin();	}
	const_iterator_edge end_edge()const		{		return m_edgeList.end();	}
	
	Edge& AddEdge( int st_idx, int ed_idx )
	{
		assert( HasNode( st_idx ) );
		assert( HasNode( ed_idx ) );
		Edge& e = m_edgeList.emplace_back();
		e.idx = (int)m_edgeList.size() - 1;
		e.source = st_idx;
		e.destination = ed_idx;

		Node& st = m_nodeList[st_idx];
		e.next_edge_idx = st.start_edge_idx;//reverse order
		st.start_edge_idx = e.idx;

		Node& ed = m_nodeList[ed_idx];
		e.next_inverse_edge_idx = ed.start_inverse_edge_idx;
		ed.start_inverse_edge_idx = e.idx;
		return e;
	}
	Edge* GetNextEdge( const Edge& e )	{		return const_cast<Edge*>( const_cast<const SparseGraph*>( this )->GetNextEdge( e ) );	}
	Node* GetNode( int idx )			{		return const_cast<Node*>( const_cast<const SparseGraph*>( this )->GetNode( idx ) );	}
	Edge* GetNextEdge( const Node& p )	{		return const_cast<Edge*>( const_cast<const SparseGraph*>( this )->GetNextEdge( p ) );	}
	
	bool HasNode( int idx )const		{		return idx >= 0 && idx < m_nodeList.size();	}
	const Node* GetNode( int idx )const	{		return &m_nodeList[idx];	}
	const Edge* GetNextEdge( const Node& p )const
	{
		if( p.start_edge_idx == -1 )
			return nullptr;
		else
			return &m_edgeList[p.start_edge_idx];
	}
	const Edge* GetNextEdge( const Edge& e )const
	{
		if( e.next_edge_idx == -1 )
			return nullptr;
		else
			return &m_edgeList[e.next_edge_idx];
	}
	size_t CountOutDegree( const Node& p )const
	{
		size_t n = 0;
		for( int edge_idx = p.start_edge_idx; edge_idx != -1; edge_idx = m_edgeList[edge_idx].next_edge_idx )
			++n;
		return n;
	}

	size_t size_node()const	{		return m_nodeList.size();	}
	size_t size_edge()const	{		return m_edgeList.size();	}

	void clear()
	{
		m_nodeList.clear();
		m_edgeList.clear();
	}
	void reserve( size_t n, size_t m )
	{
		m_nodeList.reserve( n );
		m_edgeList.reserve( m );
	}
	void resize( size_t n )
	{
		m_nodeList.resize( n );
		int idx = 0;
		for( auto& e : m_nodeList )
			e.idx = idx++;
	}
	
	//Option

	Node& AddNode()
	{
		auto& p = m_nodeList.emplace_back();
		p.idx = (int)m_nodeList.size() - 1;
		return p;
	}

	//inverse edge

	Edge* GetNextInverseEdge( const Node& p ){		return const_cast<Edge*>( const_cast<const SparseGraph*>( this )->GetNextInverseEdge( p ) );	}
	Edge* GetNextInverseEdge( const Edge& e ){		return const_cast<Edge*>( const_cast<const SparseGraph*>( this )->GetNextInverseEdge( e ) );	}
	const Edge* GetNextInverseEdge( const Node& p )const
	{
		if( p.start_inverse_edge_idx == -1 )
			return nullptr;
		else
			return &m_edgeList[p.start_inverse_edge_idx];
	}
	const Edge* GetNextInverseEdge( const Edge& e )const
	{
		if( e.next_inverse_edge_idx == -1 )
			return nullptr;
		else
			return &m_edgeList[e.next_inverse_edge_idx];
	}
	size_t CountInDegree( const Node& p )const
	{
		size_t n = 0;
		for( int edge_idx = p.start_inverse_edge_idx; edge_idx != -1; edge_idx = m_edgeList[edge_idx].next_inverse_edge_idx )
			++n;
		return n;
	}

	//sub class

	class Node :public _Node
	{
	protected:
		int start_edge_idx = -1;
		int start_inverse_edge_idx = -1;
	public:
		friend class SparseGraph;
		Node() :_Node()
		{}
	};
	class Edge :public _Edge
	{
	protected:
		int next_edge_idx = -1;
		int next_inverse_edge_idx = -1;
	public:
		friend class SparseGraph;
		Edge() :_Edge()
		{}
	};
};
static_assert( graph_type<SparseGraph<BasicNode, BasicEdge>> );
}