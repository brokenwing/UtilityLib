#pragma once
#include "pch.h"
#include "Traits.h"
#include <list>
#include <memory>

namespace Util
{
//red-black tree with rank search: https://en.wikipedia.org/wiki/Red%E2%80%93black_tree#loopInvariantI
template <typename T, typename _comp = std::less<T>, typename _allo = std::allocator<T>>
class RBtree
{
public:
	using key_type = T;
	using key_comp = _comp;
	using allocator_type = _allo;
private:
	class Node;
	using list_allocator_type = std::allocator_traits<allocator_type>::template rebind_alloc<Node>;
	using list_type = std::list<Node, list_allocator_type>;
public:
	using iterator = typename list_type::iterator;
	using const_iterator = typename list_type::const_iterator;

private:
	enum struct tColor
	{
		kBlack = 0,
		kRed = 1,
	};
	class Node
	{
	private:
		iterator parent;
		iterator child[2];
		unsigned int tot = 1;
		tColor color = tColor::kRed;
		key_type key;

	public:
		friend class RBtree;
		Node()
		{}
		Node( iterator end, const key_type& val ) :parent( end ), child{ end,end }, key( val )
		{}
		const key_type& get()const noexcept
		{
			return key;
		}
	};
	list_type data;
	iterator root;

public:
	RBtree()
	{
		root = data.end();
	}
	~RBtree()
	{}
	iterator begin()			{		return data.begin();	}
	iterator end()				{		return data.end();	}
	const_iterator begin()const	{		return data.cbegin();	}
	const_iterator end()const	{		return data.cend();	}
	std::size_t size()const noexcept	{		return data.size();	}
	bool empty()const noexcept	{		return data.empty();	}
	void clear()
	{
		data.clear();
		root = data.end();
	}
	int get_rank( const_iterator me )const
	{
		assert( me != data.end() );
		int rank = me->child[0] == data.end() ? 1 : me->child[0]->tot + 1;
		while( me != root )
		{
			auto parent = me->parent;
			assert( parent != data.end() );
			if( parent->child[1] == me )
				rank += parent->child[0] == data.end() ? 1 : 1 + parent->child[0]->tot;
			me = parent;
		}
		return rank;
	}
	//1-n
	iterator find_rank( int rank )
	{
		if( rank<1 || rank>data.size() || root == end() )
			return end();
		return find_rank( root, rank );
	}
	//1-n
	const_iterator find_rank( int rank )const
	{
		if( rank<1 || rank>data.size() || root == end() )
			return end();
		return find_rank( root, rank );
	}
	const_iterator lower_bound( const key_type& val )const
	{
		auto [it, flag] = lower_bound( root, val );
		return flag ? it : end();
	}
	iterator lower_bound( const key_type& val )
	{
		auto [it, flag] = lower_bound( root, val );
		return flag ? it : end();
	}
	const_iterator find( const key_type& val )const
	{
		if( root == data.end() )
			return data.end();
		auto [it, flag] = find( root, val );
		return flag == -1 ? it : data.end();
	}
	iterator find( const key_type& val )
	{
		if( root == data.end() )
			return data.end();
		auto [it, flag] = find( root, val );
		return flag == -1 ? it : data.end();
	}
	iterator insert( const key_type& val )
	{
		if( root == data.end() )
		{
			data.emplace_back( data.end(), val );
			root = data.begin();
			return root;//default tot = 1
		}
		else
		{
			auto [me, flag] = find( root, val );
			if( flag == -1 )//exist
				return me;
			auto parent = me;
			me = data.emplace( flag ? ++me : me, data.end(), val );

			me->parent = parent;
			parent->child[flag] = me;
			update( parent, 1 );
			insert_loop( me );
			return me;
		}
	}
	void erase( const key_type& val )
	{
		if( root != data.end() )
		{
			auto [me, flag] = find( root, val );
			if( flag == -1 )
				erase( me );
		}
	}
	void erase( iterator pos )
	{
		assert( pos != data.end() );
		if( pos == root && size() == 1 )
		{
			clear();
			return;
		}
		//move next to current pos
		if( pos->child[0] != data.end() && pos->child[1] != data.end() )
		{
			auto prev = pos++;
			assert( pos != data.end() );
			prev->key = pos->key;
		}
		//delete pos with one child (no black)
		if( pos->child[0] == data.end() && pos->child[1] == data.end() )
		{
			auto parent = pos->parent;
			assert( parent != data.end() );
			const bool dir = parent->child[1] == pos;//being deleted
			parent->child[dir] = data.end();
			auto c = pos->color;
			data.erase( pos );
			update( parent, -1 );
			if( c == tColor::kBlack )//delete red is ok
				erase_loop( parent, dir );
		}
		else
		{
			const bool subdir = pos->child[1] != data.end();//only red, delete ok
			pos->key = pos->child[subdir]->key;
			data.erase( pos->child[subdir] );
			pos->child[subdir] = data.end();
			update( pos, -1 );
		}
	}

	bool check()const
	{
		return check( root ).first;
	}
	std::string print()const
	{
		std::stringstream ss;
		print( root, ss, [&ss] ( const key_type& v )
		{
			ss << v;
			return "";
		} );
		return ss.str();
	}
	template <typename PrintKey>
	std::string print( const PrintKey f )const
	{
		std::stringstream ss;
		print( root, ss, f );
		return ss.str();
	}

private:
	std::pair<iterator, bool> lower_bound( iterator p, const key_type& val )const
	{
		iterator ret;
		bool flag = false;
		while( p != data.end() )
		{
			if( key_comp()( p->key, val ) )//right
			{
				p = p->child[1];
			}
			else if( key_comp()( val, p->key ) )//left
			{
				flag = true;
				ret = p;
				p = p->child[0];
			}
			else//eq
				return { p,true };
		}
		return { ret,flag };
	}
	iterator find_rank( iterator me, int rank )const
	{
		while( true )
		{
			assert( me != data.end() );
			const int cur = me->child[0] != data.end() ? me->child[0]->tot : 0;
			if( cur >= rank )
				me = me->child[0];
			else if( cur + 1 == rank )
				return me;
			else
			{
				me = me->child[1];
				rank -= cur + 1;
			}
		}
		assert( 0 );
	}
	template <typename Func>
	void print( iterator me, std::stringstream& ss, const Func func )const
	{
		if( me == data.end() )
			return;
		ss << func( me->key ) << ' ' << ( me->color == tColor::kBlack ? 'B' : 'R' ) << ' ' << me->tot << ':';
		for( auto c : me->child )
			if( c == data.end() )
				ss << "NULL ";
			else
				ss << func( c->key ) << ' ';
		ss << '\n';
		for( auto c : me->child )
			print( c, ss, func );
	}
	//<ok,black cnt>
	std::pair<bool, int> check( const_iterator p )const
	{
		if( p != data.end() )
		{
			if( p->tot != cnt( p ) )
				return { false,0 };
			//compare
			if( p->child[0] != data.end() && !key_comp()( p->child[0]->key, p->key ) )
				return { false,0 };
			if( p->child[1] != data.end() && !key_comp()( p->key, p->child[1]->key ) )
				return { false,0 };
			//red-red NG
			if( p->color == tColor::kRed )
			{
				for( auto c : p->child )
					if( c != data.end() && c->color == p->color )
						return { false,0 };
			}
			//same black
			auto [x1, x2] = check( p->child[0] );
			auto [y1, y2] = check( p->child[1] );
			if( !x1 || !y1 || x2 != y2 )
				return { false,0 };
			return { true,x2 + ( p->color == tColor::kBlack ) };
		}
		return { true,0 };
	}
	void erase_loop( iterator me, bool dir )
	{
		//me->child[dir] is black
		while( me != data.end() )//case 2
		{
			auto ch = me->child[!dir];
			assert( ch != data.end() );
			auto inner = ch->child[dir];
			auto outter = ch->child[!dir];
			auto c_in = inner == data.end() ? tColor::kBlack : inner->color;
			auto c_out = outter == data.end() ? tColor::kBlack : outter->color;
			if( c_in == tColor::kBlack && c_out == tColor::kBlack )
			{
				if( me->color == tColor::kBlack )
				{
					if( ch->color == tColor::kBlack )//case 1
					{
						ch->color = tColor::kRed;
						auto parent = me->parent;
						dir = parent != data.end() ? parent->child[1] == me : false;
						me = parent;
						continue;
					}
					else//case 3
					{
						rotate( ch );
						me->color = tColor::kRed;
						ch->color = tColor::kBlack;
						continue;
					}
				}
				else//case 4
				{
					assert( ch->color == tColor::kBlack );
					me->color = tColor::kBlack;
					ch->color = tColor::kRed;
					break;
				}
			}
			else if( c_in == tColor::kRed )//case 5
			{
				assert( ch->color == tColor::kBlack );
				assert( inner != data.end() );
				rotate( inner );
				inner->color = tColor::kBlack;
				ch->color = tColor::kRed;
				continue;
			}
			else//case 6
			{
				rotate( ch );
				ch->color = me->color;
				me->color = tColor::kBlack;
				assert( outter != data.end() );
				outter->color = tColor::kBlack;
				break;
			}
		}
	}
	void insert_loop( iterator me )
	{
		assert( me != data.end() );
		auto parent = me->parent;
		while( true )
		{
			if( parent == data.end() )//case 3
				break;
			if( parent->color == tColor::kBlack )//case 1
				break;
			auto grandparent = parent->parent;

			if( grandparent != data.end() )
			{
				auto uncle = grandparent->child[grandparent->child[0] == parent];
				if( uncle != data.end() && parent->color == tColor::kRed && uncle->color == tColor::kRed )//case 2
				{
					assert( grandparent->color == tColor::kBlack );
					parent->color = tColor::kBlack;
					uncle->color = tColor::kBlack;
					grandparent->color = tColor::kRed;
					me = grandparent;
					parent = me->parent;
					continue;
				}
				//uncle is black or empty
				else if( ( me == parent->child[1] ) != ( grandparent->child[1] == parent ) )//case 5: LR or RL
				{
					rotate( me );
					std::swap( me, parent );
				}
				//case 6
				rotate( parent );
				parent->color = tColor::kBlack;
				grandparent->color = tColor::kRed;
				break;
			}
			else//case 4
			{
				assert( parent->color == tColor::kRed );
				parent->color = tColor::kBlack;
				break;
			}
		}
	}
	//lift me to parent
	void rotate( iterator me )
	{
		assert( me != data.end() );
		auto parent = me->parent;
		assert( parent != data.end() );
		if( root == parent )
			root = me;
		const bool dir = me == parent->child[1];
		parent->child[dir] = me->child[!dir];
		if( me->child[!dir] != data.end() )
			me->child[!dir]->parent = parent;

		me->child[!dir] = parent;
		auto gp = parent->parent;
		me->parent = gp;

		parent->parent = me;
		if( gp != data.end() )
			gp->child[gp->child[1] == parent] = me;

		parent->tot = cnt( parent );
		me->tot = cnt( me );
	}
	//<cur,-1> or <parent, 0/1>
	std::pair<iterator, int> find( iterator p, const key_type& val )const
	{
		assert( p != data.end() );
		auto parent = p->parent;
		bool lr = false;
		while( p != data.end() )
		{
			if( key_comp()( p->key, val ) )//right
			{
				parent = p;
				p = p->child[lr = 1];
			}
			else if( key_comp()( val, p->key ) )//left
			{
				parent = p;
				p = p->child[lr = 0];
			}
			else//eq
				return { p,-1 };
		}
		return { parent,lr };
	}
	unsigned int cnt( const_iterator me )const
	{
		return 1 + ( me->child[0] != data.end() ? me->child[0]->tot : 0 ) + ( me->child[1] != data.end() ? me->child[1]->tot : 0 );
	}
	//from me to root
	void update( iterator me, const int det )
	{
		while( me != data.end() )
		{
			me->tot += det;
			me = me->parent;
		}
	}
};


template <typename T, typename _Comp = std::less<T>, typename _Alloc = std::allocator<T>>
class RBtreeSet :public RBtree<_Tset_traits<T, _Comp, _Alloc, false>>
{};
}