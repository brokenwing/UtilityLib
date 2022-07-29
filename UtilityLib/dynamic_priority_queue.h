#pragma once
#include "CommonDef.h"
#include "Util.h"

namespace Util
{
//Must indexing element to {0,1,2,3,...}
//MaxElement (Default)
template <typename Key, typename Comparator = std::less<Key>>
class dynamic_priority_queue
{
public:
	struct Node
	{
		int idx = -1;
		Key key = Key();
	};
protected:
	std::vector<Node> heap;//binary heap works on idx 1,2,3,...
	std::vector<int> idx2pos;

public:
	dynamic_priority_queue()
	{}
	const Node& top()const	{		return heap.front();	}
	bool empty()const	{		return heap.empty();	}
	void pop()
	{
		const int n = (int)heap.size();
		swap( 0, n - 1 );
		idx2pos[heap[n - 1].idx] = -1;
		heap.pop_back();
		update_down( 1 );
	}
	void push( int idx, const Key& key )
	{
		idx2pos[idx] = (int)heap.size();
		heap.emplace_back( idx, key );
		update_up( (int)heap.size() );
	}
	void erase( int idx )
	{
		const int pos = idx2pos[idx];
		swap( pos, (int)heap.size() - 1 );
		idx2pos[heap[(int)heap.size() - 1].idx] = -1;
		heap.pop_back();
		update_up( pos + 1 );
		update_down( pos + 1 );
	}
	void update_priority( int idx, const Key& key )
	{
		heap[idx2pos[idx]].key = key;
		update_up( idx2pos[idx] + 1 );
		update_down( idx2pos[idx] + 1 );
	}
	size_t size()const	{		return heap.size();	}
	void resize( size_t n )
	{
		heap.clear();
		heap.reserve( n );
		idx2pos.assign( n, -1 );
	}
	bool exists( int idx )const	{		return idx2pos[idx] != -1;	}
protected:
	//heap[X]->heap[X-1] since it works from index 1
	void swap( int a, int b )
	{
		assert( heap[a].idx >= 0 && heap[b].idx >= 0 );
		std::swap( heap[a], heap[b] );
		XORswap( idx2pos[heap[a].idx], idx2pos[heap[b].idx] );
	}
	//heap[X]->heap[X-1] since it works from index 1
	void update_down( int pos )
	{
		const int len = (int)heap.size();
		int i;
		while( ( pos << 1 ) <= len )
		{
			i = pos << 1;
			if( ( i < len ) && Comparator()( heap[i - 1].key, heap[i].key ) )
				i++;
			if( Comparator()( heap[pos - 1].key, heap[i - 1].key ) )
				swap( pos - 1, i - 1 );
			else
				break;
			pos = i;
		}
	}
	//heap[X]->heap[X-1] since it works from index 1
	void update_up( int pos )
	{
		if( pos > (int)heap.size() )
			return;
		int i;
		while( ( pos >> 1 ) > 0 )
		{
			i = pos >> 1;
			if( Comparator()( heap[i - 1].key, heap[pos - 1].key ) )
				swap( pos - 1, i - 1 );
			else
				break;
			pos = i;
		}
	}
};
}