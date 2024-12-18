#pragma once
#include "CommonDef.h"
#include <vector>

namespace Util
{
//²¢²é¼¯ (General)
template <typename T = int, typename Hasher = std::hash<T>, typename isEQ = std::equal_to<T>>
class DisjointSet
{
private:
	LFA::unordered_map<T, T, Hasher, isEQ> m_id2parent;// <id, father>

	T find( const T id )
	{
		auto node = m_id2parent.find( id );
		if( node == m_id2parent.end() )
			m_id2parent[id] = id;
		else if( !isEQ()( node->first, node->second ) )//is not root
		{
			node->second = find( node->second );//set to root
		}
		return m_id2parent[id];
	}
public:
	DisjointSet()
	{}
	DisjointSet( size_t size )
	{
		reserve( size );
	}
	~DisjointSet()
	{}

	typename decltype( m_id2parent )::const_iterator begin()const noexcept
	{
		return m_id2parent.begin();
	}
	typename decltype( m_id2parent )::const_iterator end()const noexcept
	{
		return m_id2parent.end();
	}
	std::size_t size()const
	{
		return m_id2parent.size();
	}

	bool isSameGroup( T A, T B )
	{
		A = find( A );
		B = find( B );
		return isEQ()( A, B );
	}
	T GetRoot( const T id )
	{
		return find( id );
	}
	size_t Count()
	{
		ComputeRootOfEachElement();
		int tot = 0;
		for( auto& e : m_id2parent )
			tot += e.first == e.second;
		return tot;
	}

	void Merge( T A, T B )
	{
		A = find( A );
		B = find( B );
		if( !isEQ()( A, B ) )
			m_id2parent[A] = B;
	}
	void ComputeRootOfEachElement()
	{
		for( auto& e : m_id2parent )
		{
			e.second = find( e.first );
		}
	}
	void clear() noexcept
	{
		m_id2parent.Clear();
	}
	void reserve( size_t size )
	{
		m_id2parent.reserve( size );
	}
};

//range is limited, only for int
class FastDisjointSet
{
private:
	std::vector<int> m_id2parent;// [id]=parent

	int find( const int id )
	{
		int& node = m_id2parent[id];
		if( id != node )//is not root
		{
			node = find( node );//set to root
		}
		return node;
	}
public:
	FastDisjointSet()
	{}
	FastDisjointSet( size_t size )
	{
		resize( size );
	}
	~FastDisjointSet()
	{}

	typename decltype( m_id2parent )::const_iterator begin()const noexcept
	{
		return m_id2parent.begin();
	}
	typename decltype( m_id2parent )::const_iterator end()const noexcept
	{
		return m_id2parent.end();
	}
	std::size_t size()const
	{
		return m_id2parent.size();
	}

	bool isSameGroup( int A, int B )
	{
		A = find( A );
		B = find( B );
		return A == B;
	}
	int GetRoot( const int id )
	{
		return find( id );
	}
	//number of group
	size_t Count()
	{
		ComputeRootOfEachElement();
		int tot = 0;
		const int n = static_cast<int>( m_id2parent.size() );
		for( int i = 0; i < n; ++i )
			tot += i == m_id2parent[i];
		return tot;
	}

	void Merge( int A, int B )
	{
		A = find( A );
		B = find( B );
		if( A != B )
			m_id2parent[A] = B;
	}
	void ComputeRootOfEachElement()
	{
		for( int&p : m_id2parent )
		{
			p = find( p );
		}
	}
	void resize( size_t size )
	{
		m_id2parent.resize( size );
		for( int i = 0; i < (int)size; i++ )
			m_id2parent[i] = i;
	}
};
}