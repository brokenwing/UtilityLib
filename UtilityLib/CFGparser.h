#pragma once
#include <span>
#include <string>
#include <string_view>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <optional>

namespace CFG
{
static constexpr int StartNonTerminal = 0;
static constexpr int Seperator = -1;

class Grammar
{
public:
	struct Rule
	{
		std::string desc = "";//debug
		int idx = -1;//internal
		int dom = -1;
		std::span<const int> dest;
		bool isPrint = true;
	};

private:
	int terminal_offset = 1000000;
	std::vector<int> seq;//data (ex. A->BCD)
	std::vector<Rule> relation;
	std::vector<std::span<const Rule>> dom2relation;
	std::map<int, std::string> terminal2str;//for visualization
	std::map<int, std::string> nonterminal2str;//for generating desc
	
public:
	Grammar()
	{}
	~Grammar()
	{}
	Grammar( const Grammar& ) = delete;
	Grammar& operator=( const Grammar& ) = delete;

	const Rule& GetRule( const int idx )const
	{
		return relation[idx];
	}
	decltype( dom2relation )::value_type GetRuleListByDom( const int dom )const
	{
		return dom2relation[dom];
	}
	int GetTerminalOffset()const noexcept
	{
		return terminal_offset;
	}
	void SetTerminalOffset( const int val )noexcept
	{
		assert( val > 0 );
		terminal_offset = val;
	}
	void SetNonTerminalString( int id, const std::string& str )
	{
		nonterminal2str[id] = str;
	}
	void SetTerminalString( int id, const std::string& str )
	{
		terminal2str[id] = str;
	}
	//use SetTerminalString to set str
	std::string toString( std::span<const int> text, const char* missing = "?" )const
	{
		std::string s;
		s.reserve( text.size() );
		for( int id : text )
		{
			auto r = terminal2str.find( id );
			s += ( r == terminal2str.end() ) ? missing : r->second;
		}
		return s;
	}

	void Initialize()
	{
		//seq->relation
		auto bg = seq.begin();
		for( auto & e:relation )
		{
			auto x = bg;
			while( *bg != Seperator )
				++bg;
			auto y = bg;
			e.dest = std::span<const int>( x, y );
			++bg;
		}
		//count nonterminal
		int max_id = 0;
		for( int val : seq )
		{
			max_id = std::max( max_id, isTerminal( val ) ? 0 : val );
		}
		dom2relation.clear();
		dom2relation.resize( max_id + 1 );
		//sort
		std::stable_sort( relation.begin(), relation.end(), [] ( auto& l, auto& r )->bool
		{
			return l.dom < r.dom;
		} );
		//set index
		for( int i = 0; auto & e:relation )
			e.idx = i++;
		//calc range
		std::map<int, std::pair<int, int>> dom2range;
		for( int i = 0; i < dom2relation.size(); i++ )
			dom2range[i] = { terminal_offset,0 };
		for( int i = 0; i < relation.size(); i++ )
		{
			auto& e = dom2range[relation[i].dom];
			e.first = std::min( e.first, i );
			e.second = std::max( e.second, i );
		}
		for( auto [dom, range] : dom2range )
			dom2relation[dom] = decltype( dom2relation )::value_type( relation.begin() + range.first, relation.begin() + range.second + 1 );
		//final
		GenerateDescription();
	}
	//auto split
	void AddTerminalList( int dom, std::initializer_list<int> q, const std::string& desc = "", bool isPrint = true )
	{
		for( int val : q )
			AddNonTerminal( dom, { toTerminal( val ) }, desc, isPrint );
	}
	//do not split
	void AddTerminalWord( int dom, std::vector<int> q, const std::string& desc = "", bool isPrint = true )
	{
		assert( !q.empty() );
		for( auto& e : q )
			e = toTerminal( e );
		AddNonTerminal( dom, q, desc, isPrint );
	}
	void AddNonTerminal( int dom, const std::vector<int>& q, const std::string& desc = "", bool isPrint = true )
	{
		assert( !q.empty() );
		seq.insert( seq.end(), q.begin(), q.end() );
		seq.emplace_back( Seperator );
		auto& e = relation.emplace_back();
		e.dom = dom;
		e.desc = desc;
		e.isPrint = isPrint;
	}
	int toTerminal( const int val )const noexcept
	{
		return val + ( val >= terminal_offset ? 0 : terminal_offset );
	}
	
	//same logic not data
	void DeepCopy( const Grammar& from )
	{
		clear();
		this->terminal2str = from.terminal2str;
		this->nonterminal2str = from.nonterminal2str;
		this->terminal_offset = from.terminal_offset;
		this->seq.reserve( from.seq.size() );
		this->relation = from.relation;
		for( auto& e : relation )
		{
			for( int val : e.dest )
				seq.push_back( val );
			seq.emplace_back( Seperator );
		}
		this->Initialize();
	}
	void clear()
	{
		seq.clear();
		relation.clear();
		dom2relation.clear();
		terminal2str.clear();
	}
	bool isTerminal( int val )const noexcept
	{
		return val >= terminal_offset;
	}

private:
	void GenerateDescription()
	{
		nonterminal2str[StartNonTerminal] = "S";
		for( auto& rule : relation )
			if( rule.desc == "" )
			{
				rule.desc.reserve( ( rule.dest.size() + 1 ) * 6 + 2 );//assume average sizeof word is 6
				rule.desc = nonterminal2str[rule.dom] + " =";
				for( int id : rule.dest )
				{
					rule.desc += ' ';
					rule.desc += isTerminal( id ) ? terminal2str[id - terminal_offset] : nonterminal2str[id];
				}
				rule.desc.shrink_to_fit();
			}
	}
};



class CFGparser
{
public:
	struct State
	{
		int rule_idx = -1;
		int pos = 0;//string start
		int offset = 0;//symbol offset

		bool operator<( const State& other )const noexcept
		{
			return std::make_tuple( rule_idx, pos, offset ) < std::make_tuple( other.rule_idx, other.pos, other.offset );
		}
	};
	struct Phase
	{
		int idx = -1;
		std::set<State> used;
		std::vector<State> q;
	};

private:
	Grammar grammar;
	std::vector<int> text;

	std::vector<Phase> dp;
	struct FullState :public State
	{
		int pos_end = 0;//end of matching string

		FullState()
		{}
		FullState( int dp_idx, const State& s ) :State( s ), pos_end( dp_idx )
		{}
		bool operator<( const FullState& other )const noexcept
		{
			return std::make_tuple( rule_idx, pos, pos_end, offset ) < std::make_tuple( other.rule_idx, other.pos, other.pos_end, other.offset );
		}
	};
	struct ExprNode:public FullState
	{
		int idx = -1;//multiway tree to binary tree
		int left = -1;//child
		int right = -1;//brother
	};
	std::map<FullState, int> state2idx;
	std::vector<ExprNode> expressionDAG;

public:
	CFGparser()
	{}

	void SetGrammar( const Grammar& g )
	{
		grammar.DeepCopy( g );
	}
	void SetText( const decltype( text )& val )
	{
		text = val;
	}
	void clear()
	{
		dp.clear();
		state2idx.clear();
		expressionDAG.clear();
	}
	bool Parse( const Grammar& g, const decltype( text )& _text )
	{
		SetGrammar( g );
		SetText( _text );
		return Parse();
	}
	bool Parse( const decltype( text )& _text )
	{
		SetText( _text );
		return Parse();
	}
	bool Parse()
	{
		clear();
		dp.resize( text.size() + 1 );
		for( int idx = 0; auto & e:dp )
			e.idx = idx++;
		//init
		for( auto& it : grammar.GetRuleListByDom( StartNonTerminal ) )
		{
			State x{ it.idx,0,0 };
			tryAddState( dp.front(), x );
		}
		//dp
		for( int i = 0; i <= text.size(); ++i )
		{
			auto& cur_dp = dp[i];
			for( int j = 0; j < cur_dp.q.size(); ++j )
			{
				auto cur = cur_dp.q[j];
				if( isComplete( cur ) )
				{
					assert( cur.pos < i );
					for( auto& e : dp[cur.pos].q )
					{
						if( isComplete( e ) )
							continue;
						const int nxt_id = GetNextSymbol( e );
						if( nxt_id == grammar.GetRule( cur.rule_idx ).dom )
						{
							auto tmp = e;
							++tmp.offset;
							tryAddState( cur_dp, tmp );
							addLink( FullState( cur_dp.idx, tmp ), FullState( cur_dp.idx, cur ), FullState( cur.pos, e ) );
						}
					}
				}
				else
				{
					const int id = GetNextSymbol( cur );
					if( grammar.isTerminal( id ) )
					{
						if( i < text.size() && id - grammar.GetTerminalOffset() == text[i] )
						{
							auto tmp = cur;
							++tmp.offset;
							tryAddState( dp[i + 1], tmp );
							addLink( FullState( i + 1, tmp ), std::nullopt, FullState( cur_dp.idx, cur ) );
						}
					}
					else
					{
						for( auto& e : grammar.GetRuleListByDom( id ) )
						{
							State tmp{ .rule_idx = e.idx,.pos = i,.offset = 0 };
							tryAddState( cur_dp, tmp );
						}
					}
				}
			}
		}
		return dp[text.size()].used.find( GetFinalState() ) != dp[text.size()].used.end();
	}

	std::span<const int> GetText( const FullState& p )const
	{
		assert( p.pos <= p.pos_end );
		return std::span<const int>( text.begin() + p.pos, text.begin() + p.pos_end );
	}
	//after parse
	int GetRoot()const
	{
		auto r = state2idx.find( FullState( (int)text.size(), GetFinalState() ) );
		return ( r == state2idx.end() ) ? -1 : r->second;
	}
	//after parse
	std::string toExprTreeString( const int root )const
	{
		std::ostringstream ss;
		toExprTreeString( ss, root );
		return ss.str();
	}
	//after parse
	void toExprTreeString( std::ostringstream& ss, const int root, const int deep = 1 )const
	{
		auto& me = expressionDAG[root];
		ss << me.idx << ": " << grammar.GetRule( me.rule_idx ).desc << " -> " << grammar.toString( GetText( me ) ) << '\n';
		for( int p = root; p != -1; p = expressionDAG[p].right )
		{
			const int child = expressionDAG[p].left;
			if( child != -1 && grammar.GetRule( expressionDAG[child].rule_idx ).isPrint )
			{
				ss << std::string( deep * 2, ' ' );
				toExprTreeString( ss, child, deep + 1 );
			}
		}
	}

private:
	State GetFinalState()const
	{
		State done;
		done.rule_idx = StartNonTerminal;
		done.pos = 0;
		done.offset = static_cast<int>( grammar.GetRule( StartNonTerminal ).dest.size() );
		return done;
	}
	int GetNextSymbol( const State s )const
	{
		return grammar.GetRule( s.rule_idx ).dest[s.offset];
	}
	bool isComplete( const State s )const
	{
		return grammar.GetRule( s.rule_idx ).dest.size() == s.offset;
	}
	void tryAddState( Phase& p, const State val )
	{
		if( p.used.insert( val ).second )
		{
			p.q.emplace_back( val );
			tryAddExprNode( FullState( p.idx, val ) );
		}
	}
	void tryAddExprNode( const FullState& s )
	{
		if( auto it = state2idx.insert( std::make_pair( s, -1 ) ); it.second )
		{
			it.first->second = (int)expressionDAG.size();
			auto& e = expressionDAG.emplace_back( s );
			e.idx = it.first->second;
		}
	}
	void addLink( const FullState& head, const std::optional<FullState>& l, const std::optional<FullState>& r )
	{
		assert( state2idx.find( head ) != state2idx.end() );
		const int x = state2idx.find( head )->second;
		auto& e = expressionDAG[x];
		if( l.has_value() )
		{
			assert( state2idx.find( l.value() ) != state2idx.end() );
			const int y = state2idx.find( l.value() )->second;
			e.left = y;
		}
		if( r.has_value() )
		{
			assert( state2idx.find( r.value() ) != state2idx.end() );
			const int y = state2idx.find( r.value() )->second;
			e.right = y;
		}
	}
};
}
