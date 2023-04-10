#pragma once
#include "CommonDef.h"

namespace Util
{
//²¢²é¼¯
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
		LFA::unordered_set<T, Hasher, isEQ> tmp;
		for( auto& e : m_id2parent )
		{
			tmp.insert( e.second );
		}
		return tmp.size();
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
}