#pragma once
#include "Graph.h"

namespace Util::GraphTheory
{
template <node_type _Node, edge_type _Edge>
class DenseGraph
{
public:
	using Node = _Node;
	using Edge = _Edge;
	using is_sparse_graph		= std::false_type;
	using is_discrete_idx		= std::false_type;
	using is_node_addable		= std::false_type;
	using is_node_erasable		= std::false_type;
	using is_edge_erasable		= std::true_type;
	using is_edge_duplicatable	= std::false_type;
	using has_inverse_edge		= std::true_type;

protected:
	template <bool is_const_iterator>
	class _Iterator_edge;
	typedef LFA::vector<LFA::vector<Edge>> _Graph_matrix;

	LFA::vector<Node> m_nodeList;
	_Graph_matrix m_matrix;

public:
	using iterator_node = typename decltype( m_nodeList )::iterator;
	using iterator_edge = _Iterator_edge<false>;
	using const_iterator_node = typename decltype( m_nodeList )::const_iterator;
	using const_iterator_edge = _Iterator_edge<true>;

	DenseGraph()
	{}
	//Common
	
	iterator_node begin_node()				{		return m_nodeList.begin();	}
	iterator_node end_node()				{		return m_nodeList.end();	}
	const_iterator_node begin_node()const	{		return m_nodeList.begin();	}
	const_iterator_node end_node()const		{		return m_nodeList.end();	}
	iterator_edge begin_edge()				{		return iterator_edge( m_matrix );	}
	iterator_edge end_edge()				{		return iterator_edge( (int)m_matrix.size(), 0, m_matrix );	}
	const_iterator_edge begin_edge()const	{		return const_iterator_edge( m_matrix );	}
	const_iterator_edge end_edge()const		{		return const_iterator_edge( (int)m_matrix.size(), 0, m_matrix );	}

	Edge& AddEdge( int st_idx, int ed_idx )
	{
		auto& e = m_matrix[st_idx][ed_idx];
		_Reset_idx( e, abs( e.GetIdx() ) );//filp to positive
		return e;
	}

	Edge* GetNextEdge( const Edge& e )	{		return const_cast<Edge*>( const_cast<const DenseGraph*>( this )->GetNextEdge( e ) );	}
	Node* GetNode( int idx )			{		return const_cast<Node*>( const_cast<const DenseGraph*>( this )->GetNode( idx ) );	}
	Edge* GetNextEdge( const Node& p )	{		return const_cast<Edge*>( const_cast<const DenseGraph*>( this )->GetNextEdge( p ) );	}
	
	bool HasNode( int idx )const		{		return idx >= 0 && idx < m_nodeList.size();	}
	const Node* GetNode( int idx )const	{		return &m_nodeList[idx];	}
	const Edge* GetNextEdge( const Node& p )const
	{
		for( auto& e : m_matrix[p.GetIdx()] )
			if( e.GetIdx() > 0 )
				return &e;
		return nullptr;
	}
	const Edge* GetNextEdge( const Edge& e )const
	{
		auto& row = m_matrix[e.GetSource()];
		auto n = row.size();
		for( int t = e.GetDestination() + 1; t < n; ++t )
			if( auto& tmp = row[t]; tmp.GetIdx() > 0 )
				return &tmp;
		return nullptr;
	}
	size_t CountOutDegree( const Node& p )const
	{
		size_t n = 0;
		for( auto& e : m_matrix[p.GetIdx()] )
			n += e.GetIdx() > 0;
		return n;
	}

	size_t size_node()const	{		return m_nodeList.size();	}
	size_t size_edge()const
	{
		size_t n = 0;
		for( auto& row : m_matrix )
			for( auto& e : row )
				n += e.GetIdx() > 0;
		return n;
	}

	void clear()
	{
		m_nodeList.clear();
		m_matrix.clear();
	}
	void reserve( size_t n = 0, size_t m = 0 ){}
	void resize( size_t n )
	{
		m_nodeList.resize( n );
		m_matrix.resize( n );
		for( auto& e : m_matrix )
			e.resize( n );
		reset();
	}

	//inverse edge

	Edge* GetNextInverseEdge( const Node& p ){		return const_cast<Edge*>( const_cast<const DenseGraph*>( this )->GetNextInverseEdge( p ) );	}
	Edge* GetNextInverseEdge( const Edge& e ){		return const_cast<Edge*>( const_cast<const DenseGraph*>( this )->GetNextInverseEdge( e ) );	}
	const Edge* GetNextInverseEdge( const Node& p )const
	{
		for( auto& row : m_matrix )
			if( row[p.GetIdx()].GetIdx() > 0 )
				return &row[p.GetIdx()];
		return nullptr;
	}
	const Edge* GetNextInverseEdge( const Edge& e )const
	{
		auto n = m_matrix.size();
		for( int s = e.GetSource() + 1; s < n; ++s )
			if( auto& tmp = m_matrix[s][e.GetDestination()]; tmp.GetIdx() > 0 )
				return &tmp;
		return nullptr;
	}
	size_t CountInDegree( const Node& p )const
	{
		size_t n = 0;
		for( auto& row : m_matrix )
			n += row[p.GetIdx()].GetIdx() > 0;
		return n;
	}

	//Erase
	void EraseEdge( Edge& edge )
	{
		_Reset_idx( edge, -abs(edge.GetIdx()) );//reverse idx -> invalid
	}

	//Dense
	Edge* GetEdge( int st_idx, int ed_idx ){		return const_cast<Edge*>( const_cast<const DenseGraph*>( this )->GetEdge( st_idx, ed_idx ) );	}
	const Edge* GetEdge( int st_idx, int ed_idx )const
	{
		auto& e = m_matrix[st_idx][ed_idx];
		return ( e.GetIdx() > 0 ) ? &e : nullptr;
	}

private:
	void reset()
	{
		int idx = 0;
		for( auto& e : m_nodeList )
			_Reset_idx( e, idx++ );

		idx = 0;
		int row_idx = 0;
		int col_idx = 0;
		for( auto& row : m_matrix )
		{
			col_idx = 0;
			for( auto& edge : row )
			{
				_Reset_idx( edge, -( ++idx ) );
				_Reset_source( edge, row_idx );
				_Reset_destination( edge, col_idx );
				col_idx++;
			}
			row_idx++;
		}
	}

protected:
	template <bool is_const_iterator>
	class _Iterator_edge
	{
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type		= Edge;
		using difference_type	= int;
		using pointer			= std::conditional_t<is_const_iterator, const Edge*, Edge*>;
		using reference			= std::conditional_t<is_const_iterator, const Edge&, Edge&>;

	protected:
		int s = 0;
		int t = 0;
		_Graph_matrix& ref;
		
		_Iterator_edge( _Graph_matrix& mat ) : ref( mat )
		{
			if( !ref.empty() && !ref.front().empty() && ref.front().front().GetIdx() <= 0 )
				operator++();
		}
		_Iterator_edge( int s, int t, _Graph_matrix& mat ) : ref( mat ), s( s ), t( t )
		{
			if( s < ref.size() && ref[s][t].GetIdx() <= 0 )
				operator++();
		}
	public:
		friend class DenseGraph;
		_Iterator_edge& operator++()
		{
			if( s < ref.size() )
			{
				do
				{
					++t;
					if( t >= ref[s].size() )
					{
						t = 0;
						++s;
					}
				} while( s < ref.size() && ref[s][t].GetIdx() <= 0 );
			}
			return *this;
		}
		_Iterator_edge operator++( int )
		{
			_Iterator_edge retval = *this;
			++( *this );
			return retval;
		}
		bool operator==( _Iterator_edge other ) const		{			return s == other.s && t == other.t && &ref == &other.ref;		}
		bool operator!=( _Iterator_edge other ) const		{			return !( *this == other );		}
		pointer operator->() const		{			return &ref[s][t];		}
		reference operator*() const		{			return ref[s][t];		}
	};
};
static_assert( graph_type<DenseGraph<BasicNode, BasicEdge>> );
}