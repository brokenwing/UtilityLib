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
#include <functional>
#include <deque>

namespace CFG
{
static constexpr int StartNonTerminal = 0;
static constexpr int Seperator = -1;

static_assert( std::is_empty_v<std::tuple<>> );
template <class UserType = std::tuple<>>
class Grammar
{
public:
	using UserStruct = UserType;
	struct InternalStruct
	{
		int idx = -1;
		int dom = -1;
		std::span<const int> dest;
	};
	struct ExternalStruct
	{
		std::string desc = "";//debug
		bool isPrint = true;
		int mark = 0;//user mark (for convenience)
		UserStruct user_data = UserStruct();
	};
	struct Rule :public InternalStruct, public ExternalStruct
	{
		Rule( const ExternalStruct& ext = ExternalStruct() ) :InternalStruct(), ExternalStruct( ext )
		{}
	};

	enum struct tError
	{
		kSuc = 0,
		kNoTerminal,
		kUndefindedNonTerminal,
	};

private:
	int terminal_offset = 10000;
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

	const Rule& GetRule( const int idx )const												{		return relation[idx];	}
	typename decltype( dom2relation )::value_type GetRuleListByDom( const int dom )const	{		return dom2relation[dom];	}
	int GetTerminalOffset()const noexcept													{		return terminal_offset;	}
	void SetTerminalOffset( const int val )noexcept
	{
		assert( val > 0 );
		terminal_offset = val;
	}
	void SetNonTerminalString( int id, const std::string& str )	{		nonterminal2str[id] = str;	}
	void SetTerminalString( int id, const std::string& str )	{		terminal2str[id] = str;	}
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

	tError Initialize()
	{
		if( !hasTerminal() )
			return tError::kNoTerminal;//at least one terminal
		if( hasUndefindedNonTerminal() )
			return tError::kUndefindedNonTerminal;//not defined but used
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
		
		return tError::kSuc;
	}
	//auto split
	void AddTerminalList( int dom, std::initializer_list<int> q, const ExternalStruct& ext_info = {} )
	{
		for( int val : q )
			AddNonTerminal( dom, { toTerminal( val ) }, ext_info );
	}
	//do not split
	void AddTerminalWord( int dom, std::vector<int> q, const ExternalStruct& ext_info = {} )
	{
		assert( !q.empty() );
		for( auto& e : q )
			e = toTerminal( e );
		AddNonTerminal( dom, q, ext_info );
	}
	void AddNonTerminal( int dom, const std::vector<int>& q, const ExternalStruct& ext_info = {} )
	{
		assert( !q.empty() );
		seq.insert( seq.end(), q.begin(), q.end() );
		seq.emplace_back( Seperator );
		auto& e = relation.emplace_back( ext_info );
		e.dom = dom;
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
	bool isTerminal( int val )const noexcept	{		return val >= terminal_offset;	}
	void AddASCIIAsTerminal()
	{
		for( int i = 1; i < 128; i++ )
			SetTerminalString( i, std::string( 1, char( i ) ) );
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
	bool hasTerminal()const
	{
		bool flag = false;
		for( int x : seq )
			if( x != -1 )
				flag |= isTerminal( x );
		return flag;
	}
	bool hasUndefindedNonTerminal()const
	{
		std::set<int> nt_used = { StartNonTerminal };
		for( int x : seq )
			if( x != -1 && !isTerminal( x ) )
				nt_used.insert( x );
		
		std::set<int> nt_def;
		for( auto& r : relation )
			nt_def.insert( r.dom );
		return nt_used != nt_def;
	}
};
template <typename T>
concept grammar_type = std::is_base_of_v<Grammar<typename T::UserStruct>, T>;

namespace GrammarTemplate
{
template <grammar_type G>
void AddIntegerDef( G& g, const int num_id, const int digit_id, bool is_digit_defined = false )
{
	if( !is_digit_defined )
		g.AddTerminalList( digit_id, { '0','1','2','3','4','5','6','7','8','9' } );
	g.AddNonTerminal( num_id, { digit_id } );
	g.AddNonTerminal( num_id, { digit_id,num_id } );
}
//must define integer first
template <grammar_type G>
void AddRealNumberDef( G& g, const int real_num_id, const int int_id, const int dot_id )
{
	g.AddNonTerminal( real_num_id, { int_id } );
	g.AddNonTerminal( real_num_id, { int_id,dot_id,int_id } );
}
//must define real number first, set int_id if real is not including int
template <grammar_type G>
void AddScientificNotationDef( G& g, const int scientific_notation_id, const int real_num_id, const int neg_id )
{
	for( char ch : {'e', 'E'} )
	{
		g.AddNonTerminal( scientific_notation_id, { real_num_id,g.toTerminal( ch ),real_num_id } );
		g.AddNonTerminal( scientific_notation_id, { real_num_id,g.toTerminal( ch ),neg_id,real_num_id } );
	}
}
//NG: start from number
template <grammar_type G>
void AddIDdef( G& g, const int id, const int char_id, const int num_id, bool is_char_defined = false )
{
	if( !is_char_defined )
	{
		for( char c = 'a'; c <= 'z'; ++c )
			g.AddTerminalList( char_id, { c }, { "char" } );
		for( char c = 'A'; c <= 'Z'; ++c )
			g.AddTerminalList( char_id, { c }, { "char" } );
		g.AddTerminalList( char_id, { '_' }, { "char" } );
	}
	g.AddNonTerminal( id, { char_id } );
	g.AddNonTerminal( id, { char_id,id } );
	g.AddNonTerminal( id, { char_id,num_id } );
	g.AddNonTerminal( id, { char_id,num_id,id } );
}

//ref http://www.cs.ecu.edu/karl/5220/spr16/Notes/CFG/precedence.html
struct ExprPriorityConfig
{
	int expr_id = -1;
	int operator_id = -1;//non-terminal
	int mark = 1;//identify this expression
	bool unary = false;
};
//sort from low prio to high prio
//<expr_id,operator_id,unary> return main expr id
template <grammar_type G>
[[nodiscard]]int AddExprWithPriority( G& g, const std::vector<ExprPriorityConfig>& def, const int last_expr, const int compute_unit, bool parentheses = true )
{
	for( int i = 0; i < def.size(); i++ )
	{
		int next_level = last_expr;
		if( i + 1 < def.size() )
			next_level = def[i + 1].expr_id;
		const int x = def[i].expr_id;
		const int y = def[i].operator_id;
		g.AddNonTerminal( x, { next_level } );
		if( def[i].unary )
			g.AddNonTerminal( x, { y,x }, { .mark = def[i].mark } );
		else
			g.AddNonTerminal( x, { x,y,next_level }, { .mark = def[i].mark } );
	}
	g.AddNonTerminal( last_expr, { compute_unit } );
	if( parentheses && !def.empty() )
		g.AddNonTerminal( last_expr, { g.toTerminal( '(' ),def.front().expr_id,g.toTerminal( ')' ) } );
	return def.empty() ? last_expr : def.front().expr_id;
}
}

template <grammar_type T_Grammar = Grammar<>>
class CFGparser
{
public:
	using Grammar = T_Grammar;
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
	struct FullState :public State
	{
		int pos_end = 0;//end of matching string

		FullState()
		{}
		FullState( int dp_idx, const State& s ) :State( s ), pos_end( dp_idx )
		{}
		bool operator<( const FullState& other )const noexcept
		{
			return std::make_tuple( this->rule_idx, this->pos, this->pos_end, this->offset ) < std::make_tuple( other.rule_idx, other.pos, other.pos_end, other.offset );
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
	/*
	* Expr->ABCD (finish D)
	* Expr.left==D
	* Expr.right==Expr->ABCD (finish C)
	* Expr.right.left==C
	* Expr.right.right==Expr->ABCD (finish B)
	* Expr.right.right.left==B
	*/
	struct ExprNode:public FullState
	{
		int idx = -1;
		int parent = -1;//ambiguous when parsing, need to calc after parsing from root with GetChildList
		int left = -1;//child of current element
		int right = -1;//right->right->... to visit each element in dest in BW order

		int GetNextNodeIdx()const noexcept
		{
			return right;
		}
		int GetChildIdx()const noexcept
		{
			return left;
		}
	};
	std::map<FullState, int> state2idx;
	std::vector<ExprNode> expressionDAG;

public:
	CFGparser()
	{}
	~CFGparser()
	{}

	const Grammar& GetGrammar()const			{		return grammar;	}
	const decltype( text )& GetText()const		{		return text;	}
	void SetGrammar( const Grammar& g )			{		grammar.DeepCopy( g );	}
	void SetText( const decltype( text )& val )	{		text = val;	}
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
							addLink( FullState( cur_dp.idx, tmp ), FullState( cur_dp.idx, cur ), FullState( cur.pos, e ) );//<new,complete,old>
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
							addLink( FullState( i + 1, tmp ), std::nullopt, FullState( cur_dp.idx, cur ) );//<new,null,old>
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
		bool r = CalcParent();
		return r && dp[text.size()].used.find( GetFinalState() ) != dp[text.size()].used.end();
	}

	std::span<const int> GetText( const FullState& p )const
	{
		assert( p.pos <= p.pos_end );
		return std::span<const int>( text.begin() + p.pos, text.begin() + p.pos_end );
	}

	/*after parse*/

	int GetRoot()const
	{
		auto r = state2idx.find( FullState( (int)text.size(), GetFinalState() ) );
		return ( r == state2idx.end() ) ? -1 : r->second;
	}
	const FullState& GetNode( int idx )const	{		return expressionDAG[idx];	}
	int GetParent( int idx )const				{		return expressionDAG[idx].parent;	}
	//from left to right in dest
	std::vector<int> GetChildList( int idx )const
	{
		if( idx < 0 || idx >= expressionDAG.size() ) [[unlikely]]
			return {};
		std::vector<int> ret;
		ret.reserve( 8 );
		for( int p = idx; p != -1; p = expressionDAG[p].GetNextNodeIdx() )
		{
			const int child = expressionDAG[p].GetChildIdx();
			if( child != -1 )
				ret.emplace_back( child );
		}
		std::reverse( ret.begin(), ret.end() );
		return ret;
	}
	std::string toExprTreeString( const int root )const
	{
		std::ostringstream ss;
		toExprTreeString( ss, root );
		return ss.str();
	}
	void toExprTreeString( std::ostringstream& ss, const int root, const int deep = 1 )const
	{
		auto& me = expressionDAG[root];
		ss << me.idx << '(' << me.parent << ')' << ": " << grammar.GetRule( me.rule_idx ).desc << " -> " << grammar.toString( GetText( me ) ) << '\n';
		for( int p = root; p != -1; p = expressionDAG[p].GetNextNodeIdx() )
		{
			const int child = expressionDAG[p].GetChildIdx();
			if( child != -1 && grammar.GetRule( expressionDAG[child].rule_idx ).isPrint )
			{
				ss << std::string( deep * 2, ' ' );
				toExprTreeString( ss, child, deep + 1 );
			}
		}
	}

private:
	bool CalcParent()
	{
		const int root = GetRoot();
		if( root == -1 )
			return false;

		std::set<int> visit = { root };
		std::deque<int> q = { root };

		while( !q.empty() )
		{
			expressionDAG[q.front()].parent;
			for( int idx : GetChildList( q.front() ) )
			{
				expressionDAG[idx].parent = q.front();
				q.emplace_back( idx );
				if( !visit.insert( idx ).second )
					return false;
			}
			q.pop_front();
		}
		return true;
	}
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
template <typename T>
concept cfg_type = std::is_base_of_v<CFGparser<typename T::Grammar>, T>;

namespace CFGtool
{
inline std::vector<int> string2vector( const std::string& s )
{
	std::vector<int> text;
	text.reserve( s.size() );
	for( auto e : s )
		text.emplace_back( e );
	return text;
};

constexpr int ForceExpandMark = -1;//do not print this node
constexpr int UnaryOperatorExpandMark = 1;
constexpr int BinaryOperatorExpandMark = 2;
constexpr int FunctionExpandMark = 3;
//seq of node index
template <cfg_type Parser>
std::vector<int> toPostorderSequence( const Parser& cfg, const std::set<int>& targetNT )
{
	if( cfg.GetRoot() == -1 )
		return {};
	auto& g = cfg.GetGrammar();
	std::set<int> visited;//loop check
	std::vector<int> seq;
	seq.reserve( cfg.GetText().size() );
	bool isLoop = false;
	std::function<void( const int )> postorderExpand = nullptr;
	postorderExpand = [&] ( const int root )->void
	{
		if( isLoop )
			return;
		if( !visited.insert( root ).second )
		{
			isLoop = true;
			return;
		}
		auto& me = cfg.GetNode( root );
		if( auto& rule = g.GetRule( me.rule_idx ); rule.mark != ForceExpandMark && targetNT.find( rule.dom ) != targetNT.end() )
		{
			seq.emplace_back( root );
			return;
		}

		auto q = cfg.GetChildList( root );
		switch( g.GetRule( me.rule_idx ).mark )
		{
		case UnaryOperatorExpandMark:
		{
			assert( q.size() == 2 );
			postorderExpand( q[1] );//x
			postorderExpand( q[0] );//ope
			break;
		}
		case BinaryOperatorExpandMark:
		{
			assert( q.size() == 3 );
			postorderExpand( q[0] );//x
			postorderExpand( q[2] );//y
			postorderExpand( q[1] );//ope
			break;
		}
		case FunctionExpandMark:
		{
			assert( q.size() >= 1 );
			for( int i = 1; i < q.size(); ++i )
				postorderExpand( q[i] );
			postorderExpand( q[0] );//function name
			break;
		}
		default:
			for( int x : q )
				postorderExpand( x );
			break;
		}
	};

	postorderExpand( cfg.GetRoot() );
	if( isLoop )
		seq.clear();
	seq.shrink_to_fit();
	return seq;
}
template <cfg_type Parser>
std::string toPostorderExprString( const Parser& cfg, const std::set<int>& targetNT )
{
	auto q = toPostorderSequence( cfg, targetNT );
	std::ostringstream ss;
	for( int idx : q )
		ss << cfg.GetGrammar().toString( cfg.GetText( cfg.GetNode( idx ) ) ) << ',';
	return ss.str();
}

}

}
