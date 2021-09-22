#pragma once
#include "pch.h"
#include <vector>
#include <list>
#include "CommonDef.h"

namespace Util
{
template<typename _MyBlockList, bool is_const>
class _BlockList_iterator
{
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = typename _MyBlockList::value_type;
	using difference_type = int;
	using pointer = value_type*;
	using reference = std::conditional_t<is_const, const value_type&, value_type&>;

private:
	typedef std::conditional_t<is_const,
		typename _MyBlockList::list_type::const_iterator,
		typename _MyBlockList::list_type::iterator> block_iterator;
	typedef std::conditional_t<is_const,
		typename _MyBlockList::block_type::const_iterator,
		typename _MyBlockList::block_type::iterator> element_iterator;
	block_iterator m_block_begin;
	block_iterator m_block_end;//fix
	element_iterator m_elm_iter;

public:
	friend class _BlockList_iterator<_MyBlockList, true>;
	_BlockList_iterator()
	{}
	_BlockList_iterator( block_iterator st, block_iterator ed, element_iterator elm ) :
		m_block_begin( st ), m_block_end( ed ), m_elm_iter( elm )
	{}
	_BlockList_iterator( const _BlockList_iterator<_MyBlockList, false>& other )
	{
		m_block_begin = other.m_block_begin;
		m_block_end = other.m_block_end;
		m_elm_iter = other.m_elm_iter;
	}
	_BlockList_iterator& operator++()
	{
		next();
		return *this;
	}
	_BlockList_iterator& operator--()
	{
		prev();
		return *this;
	}
	_BlockList_iterator operator++( int )
	{
		auto tmp = *this;
		next();
		return tmp;
	}
	_BlockList_iterator operator--( int )
	{
		auto tmp = *this;
		prev();
		return tmp;
	}

	reference operator*()const noexcept
	{
		return *m_elm_iter;
	}
	bool operator!=( const _BlockList_iterator& other )const noexcept
	{
		return other.m_elm_iter != this->m_elm_iter || other.m_block_begin != this->m_block_begin;
	}
	bool operator==( const  _BlockList_iterator& other )const noexcept
	{
		return !( *this != other );
	}

private:
	void next()
	{
		if( m_block_begin != m_block_end && ++m_elm_iter == m_block_begin->end() && ++m_block_begin != m_block_end )
		{
			m_elm_iter = m_block_begin->begin();
		}
	}
	void prev()
	{
		if( m_block_begin == m_block_end )
			--m_block_begin;
		if( m_elm_iter == m_block_begin->begin() )
		{
			m_elm_iter = ( --m_block_begin )->end();
		}
		--m_elm_iter;
	}
};

template<typename T, template<typename> class _Alloc = DefaultAllocator>
class BlockList
{
public:
	using value_type = T;
	using block_type = std::vector<T, _Alloc<T>>;
	using list_type = std::list<block_type, _Alloc<block_type>>;

	friend class _BlockList_iterator<BlockList<T, _Alloc>, true>;
	friend class _BlockList_iterator<BlockList<T, _Alloc>, false>;

	using iterator = _BlockList_iterator<BlockList<T, _Alloc>, false>;
	using const_iterator = _BlockList_iterator<BlockList<T, _Alloc>, true>;

private:
	list_type m_data;
	size_t n = 0;

public:
	BlockList()
	{}
	BlockList( std::initializer_list<T>&& list )
	{
		n = list.size();
		if( n != 0 )
			m_data.emplace_back( list );
	}
	BlockList( std::initializer_list<std::initializer_list<T>>&& list );

	iterator begin()
	{
		return iterator( m_data.begin(), m_data.end(), m_data.front().begin() );
	}
	iterator end()
	{
		return iterator( m_data.end(), m_data.end(), m_data.back().end() );
	}
	const_iterator cbegin()const
	{
		return const_iterator( m_data.begin(), m_data.end(), m_data.front().begin() );
	}
	const_iterator cend()const
	{
		return const_iterator( m_data.end(), m_data.end(), m_data.back().end() );
	}
	const_iterator begin()const
	{
		return cbegin();
	}
	const_iterator end()const
	{
		return cend();
	}

	typename list_type::iterator block_begin()noexcept;
	typename list_type::iterator block_end()noexcept;
	typename list_type::const_iterator block_begin()const noexcept;
	typename list_type::const_iterator block_end()const noexcept;
	size_t size()const noexcept;
	size_t block_size()const noexcept;
	bool empty()const noexcept;

	typename list_type::iterator block_at( int index );
	typename list_type::const_iterator block_at( int index )const;
	typename list_type::iterator find_block( int& idx );
	typename list_type::const_iterator find_block( int& idx )const;

	T& front()
	{
		return m_data.front().front();
	}
	const T& front()const
	{
		return m_data.front().front();
	}

	T& at( int index );
	const T& at( int index )const;
	T& operator[]( int index );
	const T& operator[]( int index )const;

	void clear()
	{
		m_data.clear();
		n = 0;
	}
	void recalc_size()
	{
		n = 0;
		for( auto& e : m_data )
			n += e.size();
	}

	void merge( typename list_type::iterator from, typename list_type::iterator to );
	//merge behind
	void merge( const int block_idx_from, const int block_idx_to );
	//[begin,elm_pos-1],[elm_pos,end]
	void split( typename list_type::iterator block_pos, typename block_type::iterator elm_pos );
	//[begin,pos-1],[pos,end]
	void split( int pos );

	void push_back( const T& val, bool new_block = false )
	{
		if( new_block )
			m_data.emplace_back().emplace_back( val );
		else
		{
			if( empty() )
				m_data.emplace_back();
			m_data.back().emplace_back( val );
		}
		n++;
	}
	void move( typename list_type::iterator block_from, typename list_type::iterator block_to )
	{
		m_data.insert( block_to, std::move( *block_from ) );
		m_data.erase( block_from );
	}
	void move( int block_idx_from, int block_idx_to )
	{
		move( block_at( block_idx_from ), block_at( block_idx_to + ( block_idx_to > block_idx_from ) ) );
	}
};

template<typename T, template<typename> class _Alloc>
inline BlockList<T, _Alloc>::BlockList( std::initializer_list<std::initializer_list<T>>&& list )
{
	n = 0;
	for( auto& e : list )
	{
		if( e.size() == 0 )
			continue;
		n += e.size();
		m_data.emplace_back( e );
	}
}

template<typename T, template<typename> class _Alloc>
inline typename BlockList<T, _Alloc>::list_type::iterator BlockList<T, _Alloc>::block_begin() noexcept
{
	return m_data.begin();
}

template<typename T, template<typename> class _Alloc>
inline typename BlockList<T, _Alloc>::list_type::iterator BlockList<T, _Alloc>::block_end() noexcept
{
	return m_data.end();
}

template<typename T, template<typename> class _Alloc>
inline typename BlockList<T, _Alloc>::list_type::const_iterator BlockList<T, _Alloc>::block_begin() const noexcept
{
	return m_data.cbegin();
}

template<typename T, template<typename> class _Alloc>
inline typename BlockList<T, _Alloc>::list_type::const_iterator BlockList<T, _Alloc>::block_end() const noexcept
{
	return m_data.cend();
}

template<typename T, template<typename> class _Alloc>
inline size_t BlockList<T, _Alloc>::size() const noexcept
{
	return n;
}

template<typename T, template<typename> class _Alloc>
inline size_t BlockList<T, _Alloc>::block_size() const noexcept
{
	return m_data.size();
}

template<typename T, template<typename> class _Alloc>
inline bool BlockList<T, _Alloc>::empty() const noexcept
{
	return m_data.empty();
}

template<typename T, template<typename> class _Alloc>
inline typename BlockList<T, _Alloc>::list_type::iterator BlockList<T, _Alloc>::block_at( int index )
{
	assert( index >= 0 );
	assert( index <= (int)m_data.size() );
	auto it = m_data.begin();
	while( index-- > 0 )
		++it;
	return it;
}

template<typename T, template<typename> class _Alloc>
inline typename BlockList<T, _Alloc>::list_type::const_iterator BlockList<T, _Alloc>::block_at( int index )const
{
	assert( index >= 0 );
	assert( index <= (int)m_data.size() );
	auto it = m_data.begin();
	while( index-- > 0 )
		++it;
	return it;
}

template<typename T, template<typename> class _Alloc>
inline typename BlockList<T, _Alloc>::list_type::iterator BlockList<T, _Alloc>::find_block( int& idx )
{
	assert( idx >= 0 );
	assert( idx < (int)n );
	assert( !m_data.empty() );
	auto cur = m_data.begin();
	while( idx >= (int)cur->size() )
		idx -= (int)( ( cur++ )->size() );
	return cur;
}

template<typename T, template<typename> class _Alloc>
inline typename BlockList<T, _Alloc>::list_type::const_iterator BlockList<T, _Alloc>::find_block( int& idx )const
{
	assert( idx >= 0 );
	assert( idx < n );
	assert( !m_data.empty() );
	auto cur = m_data.begin();
	while( idx >= cur->size() )
		idx -= (int)( ( cur++ )->size() );
	return cur;
}

template<typename T, template<typename> class _Alloc>
inline T& BlockList<T, _Alloc>::at( int index )
{
	if( index < 0 || index >= (int)n )
		throw std::runtime_error( "invalid index" );
	auto cur = find_block( index );
	return cur->at( index );
}

template<typename T, template<typename> class _Alloc>
inline const T& BlockList<T, _Alloc>::at( int index )const
{
	if( index < 0 || index >= (int)n )
		throw std::runtime_error( "invalid index" );
	auto cur = find_block( index );
	return cur->at( index );
}

template<typename T, template<typename> class _Alloc>
inline T& BlockList<T, _Alloc>::operator[]( int index )
{
	auto cur = find_block( index );
	return cur->operator[]( index );
}

template<typename T, template<typename> class _Alloc>
inline const T& BlockList<T, _Alloc>::operator[]( int index )const
{
	auto cur = find_block( index );
	return cur->operator[]( index );
}

template<typename T, template<typename> class _Alloc>
void BlockList<T, _Alloc>::merge( typename list_type::iterator from, typename list_type::iterator to )
{
	assert( from != m_data.end() );
	assert( to != m_data.end() );
	to->insert( to->end(), from->begin(), from->end() );
	m_data.erase( from );
}

//merge behind
template<typename T, template<typename> class _Alloc>
void BlockList<T, _Alloc>::merge( const int block_idx_from, const int block_idx_to )
{
	merge( this->block_at( block_idx_from ), this->block_at( block_idx_to ) );
}

//[begin,elm_pos-1],[elm_pos,end]
template<typename T, template<typename> class _Alloc>
void BlockList<T, _Alloc>::split( typename list_type::iterator block_pos, typename block_type::iterator elm_pos )
{
	assert( block_pos != m_data.end() );
	if( elm_pos == block_pos->begin() || elm_pos == block_pos->end() )
		return;
	m_data.insert( block_pos, block_type( block_pos->begin(), elm_pos ) );
	block_pos->erase( block_pos->begin(), elm_pos );
}

//[begin,pos-1],[pos,end]
template<typename T, template<typename> class _Alloc>
void BlockList<T, _Alloc>::split( int pos )
{
	auto cur = find_block( pos );
	split( cur, cur->begin() + pos );
}
}