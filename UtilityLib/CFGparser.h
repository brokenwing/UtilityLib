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
#include <format>

namespace CFG
{
static constexpr int StartNonTerminal = 0;
static constexpr int Seperator = -1;

static_assert( std::is_empty_v<std::tuple<>> );
template <class user_type = std::tuple<>, typename string_elem_type = char>
class Grammar
{
public:
	using UserStruct = user_type;
	using StringElemType = string_elem_type;
	using StringType = std::basic_string<string_elem_type>;

	struct InternalStruct
	{
		int idx = -1;
		int dom = -1;
		std::span<const int> dest;
	};
	struct ExternalStruct
	{
		StringType desc;//debug
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
	std::map<int, StringType> terminal2str;//for visualization
	std::map<int, StringType> nonterminal2str;//for generating desc
	
public:
	Grammar()
	{}
	~Grammar()
	{}
	Grammar( const Grammar& ) = delete;
	Grammar& operator=( const Grammar& ) = delete;

	static constexpr StringType T( const std::string&& s )
	{
		if constexpr( std::same_as<char, StringElemType> )
			return s;
		if constexpr( std::same_as<wchar_t, StringElemType> )
			return StringType( s.begin(), s.end() );
	}

	const Rule& GetRule( const int idx )const												{		return relation[idx];	}
	typename decltype( dom2relation )::value_type GetRuleListByDom( const int dom )const	{		return dom2relation[dom];	}
	int GetTerminalOffset()const noexcept													{		return terminal_offset;	}
	void SetTerminalOffset( const int val )noexcept
	{
		assert( val > 0 );
		terminal_offset = val;
	}
	void SetNonTerminalString( int id, const StringType& str )	{		nonterminal2str[id] = str;	}
	void SetNonTerminalString( int id, const char* s )			{		nonterminal2str[id] = T( s );	}
	void SetTerminalString( int id, const StringType& str )	{		terminal2str[id] = str;	}
	void SetTerminalString( int id, const char* s )	{		terminal2str[id] = T( s );	}
	//use SetTerminalString to set str
	StringType toString( std::span<const int> text, const StringType& missing = T( "?" ) )const
	{
		StringType s;
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
		for( int i = 0; i < (int)dom2relation.size(); i++ )
			dom2range[i] = { terminal_offset,0 };
		for( int i = 0; i < (int)relation.size(); i++ )
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
			SetTerminalString( i, StringType( 1, StringElemType( i ) ) );
	}

private:
	void GenerateDescription()
	{
		nonterminal2str[StartNonTerminal] = T( "S" );
		for( auto& rule : relation )
			if( rule.desc.empty() )
			{
				rule.desc.reserve( ( rule.dest.size() + 1 ) * 6 + 2 );//assume average sizeof word is 6
				rule.desc = nonterminal2str[rule.dom] + T( " =" );
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

template <typename T = std::tuple<>>
using WGrammar = Grammar<T, wchar_t>;

template <typename T>
concept grammar_type = std::is_base_of_v<Grammar<typename T::UserStruct, char>, T> || std::is_base_of_v<Grammar<typename T::UserStruct, wchar_t>, T>;

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
			g.AddTerminalList( char_id, { c }, { g.T( "char" ) } );
		for( char c = 'A'; c <= 'Z'; ++c )
			g.AddTerminalList( char_id, { c }, { g.T( "char" ) } );
		g.AddTerminalList( char_id, { '_' }, { g.T( "char" ) } );
	}
	g.AddNonTerminal( id, { char_id } );
	g.AddNonTerminal( id, { char_id,id } );
	g.AddNonTerminal( id, { char_id,num_id } );
	g.AddNonTerminal( id, { char_id,num_id,id } );
}

//rawstr has quotes, single character quotes is not allowed in string
//quotes (more than 1 character) inside string is undefined behavior
//empty string ok
template <grammar_type G>
void AddStringDef( G& g, const int rawstr_id, const int str_id, const int left_quote_id, const int right_quote_id, const int char_id, bool is_char_defined = false )
{
	if( !is_char_defined )
		for( int i = 1; i < 128; i++ )
			if( g.toTerminal( i ) != left_quote_id && g.toTerminal( i ) != right_quote_id )
				g.AddTerminalList( char_id, { i } );
	g.AddNonTerminal( rawstr_id, { left_quote_id,str_id,right_quote_id } );
	g.AddNonTerminal( rawstr_id, { left_quote_id,right_quote_id } );
	g.AddNonTerminal( str_id, { char_id } );
	g.AddNonTerminal( str_id, { char_id,str_id } );
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
	for( int i = 0; i < (int)def.size(); i++ )
	{
		int next_level = last_expr;
		if( i + 1 < (int)def.size() )
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
	using StringElemType = typename Grammar::StringElemType;
	using StringType = typename Grammar::StringType;
	using StringStreamType = std::basic_stringstream<StringElemType>;

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
	struct ExprNode :public FullState
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
	int root = -1;
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
		root = -1;
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
		for( int i = 0; i <= (int)text.size(); ++i )
		{
			auto& cur_dp = dp[i];
			for( int j = 0; j < (int)cur_dp.q.size(); ++j )
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
						if( i < (int)text.size() && id - grammar.GetTerminalOffset() == text[i] )
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
		//find root (if more than one possible S, choose any)
		for( auto& rule : grammar.GetRuleListByDom( StartNonTerminal ) )
		{
			State done;
			done.rule_idx = rule.idx;
			done.pos = 0;
			done.offset = static_cast<int>( rule.dest.size() );
			if( dp[text.size()].used.find( done ) != dp[text.size()].used.end() )
			{
				auto r = state2idx.find( FullState( (int)text.size(), done ) );
				root = ( r == state2idx.end() ) ? -1 : r->second;
				break;
			}
		}
		const bool r = CalcParent();
		return r;
	}

	std::span<const int> GetText( const FullState& p )const
	{
		assert( p.pos <= p.pos_end );
		return std::span<const int>( text.begin() + p.pos, text.begin() + p.pos_end );
	}

	/*after parse*/

	int GetRoot()const noexcept
	{
		return root;
	}
	const FullState& GetNode( int idx )const	{		return expressionDAG[idx];	}
	int GetParent( int idx )const				{		return expressionDAG[idx].parent;	}
	//from left to right in dest
	std::vector<int> GetChildList( int idx )const
	{
		if( idx < 0 || idx >= (int)expressionDAG.size() ) [[unlikely]]
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
	StringType toExprTreeString( const int root )const
	{
		StringStreamType ss;
		toExprTreeString( ss, root );
		return ss.str();
	}
	void toExprTreeString( StringStreamType& ss, const int root, const int deep = 1 )const
	{
		auto& me = expressionDAG[root];
		ss << me.idx << '(' << me.parent << ')' << ": " << grammar.GetRule( me.rule_idx ).desc << " -> " << grammar.toString( GetText( me ) ) << '\n';
		for( int p = root; p != -1; p = expressionDAG[p].GetNextNodeIdx() )
		{
			const int child = expressionDAG[p].GetChildIdx();
			if( child != -1 && grammar.GetRule( expressionDAG[child].rule_idx ).isPrint )
			{
				ss << StringType( deep * 2, ' ' );
				toExprTreeString( ss, child, deep + 1 );
			}
		}
	}

	enum struct tError
	{
		kNoError,
		kEmptyText,
		kMidState,
		kFinalState,//match whole string but no valid start
		kUnknown,
	};
	struct ErrorInfo
	{
		tError type = tError::kUnknown;
		int pos = -1;//start from 1
		StringType ch;
		StringType text;
		StringType expect_rawstr;
		StringType usr_msg_without_col;
		StringType usr_msg;
		std::vector<StringType> debug_msg;//each state of dp[pos]
	};
	ErrorInfo ParseErrorReport()const
	{
		if( text.empty() )
			return ErrorInfo{ .type = tError::kEmptyText };

		if( GetRoot() == -1 )
		{
			ErrorInfo ret;

			assert( dp.size() == text.size() + 1 );
			int ng_pos = static_cast<int>( dp.size() ) - 1;
			while( ng_pos > 0 && dp[ng_pos].q.empty() )
				--ng_pos;
			ret.pos = ng_pos + 1;

			if( ng_pos < (int)text.size() && ng_pos >= 0 )
			{
				int ch[1] = { text[ng_pos] };
				ret.ch = grammar.toString( ch );
			}
			else
				ret.ch = grammar.T( "End" );
			ret.text = grammar.toString( GetText() );
			ret.type = ( ng_pos == text.size() ) ? tError::kFinalState : tError::kMidState;

			std::vector<int> expect_charlist;
			ret.debug_msg.reserve( dp[ng_pos].q.size() );
			for( auto it = dp[ng_pos].q.rbegin(); it != dp[ng_pos].q.rend(); ++it )
			{
				int state = -1;//1-complete, 2-read char, 3-predict
				StringType state_str;
				if( isComplete( *it ) )
				{
					state = 1;
					state_str = grammar.T( "complete" );
				}
				else if( grammar.isTerminal( GetNextSymbol( *it ) ) )
				{
					state = 2;
					state_str = grammar.T( "expect" );
				}
				else
				{
					state = 3;
					state_str = grammar.T( "predict" );
				}

				if( state == 2 )
					expect_charlist.emplace_back( GetNextSymbol( *it ) - grammar.GetTerminalOffset() );
				const auto& cur_rule_desc = grammar.GetRule( it->rule_idx ).desc;
				std::span<const int> match_text( GetText().begin() + it->pos, GetText().begin() + ng_pos );
				auto match_str = grammar.toString( match_text );
				if( match_str.empty() )
					match_str = grammar.T( "nothing" );
				else
					match_str = grammar.T( "'" ) + match_str + grammar.T( "'" );

				if constexpr( std::same_as<char, StringElemType> )
					ret.debug_msg.emplace_back( std::format( "Rule {}:{} is {}, matches {} with {} symbols.", it->rule_idx, cur_rule_desc, state_str, match_str, it->offset ) );
				if constexpr( std::same_as<wchar_t, StringElemType> )
					ret.debug_msg.emplace_back( std::format( L"Rule {}:{} is {}, matches {} with {} symbols.", it->rule_idx, cur_rule_desc, state_str, match_str, it->offset ) );
			}
			//remove dup
			std::sort( expect_charlist.begin(), expect_charlist.end() );
			expect_charlist.erase( std::unique( expect_charlist.begin(), expect_charlist.end() ), expect_charlist.end() );

			std::vector<StringType> expect_strlist;
			expect_strlist.reserve( expect_charlist.size() );
			for( int ch : expect_charlist )
			{
				int tmp[1] = { ch };
				expect_strlist.emplace_back( grammar.toString( tmp ) );
			}
			std::sort( expect_strlist.begin(), expect_strlist.end() );

			StringType expect_str, special_str;
			expect_str.reserve( expect_strlist.size() );
			for( auto& s : expect_strlist )
				expect_str += s;
			ret.expect_rawstr = expect_str;
			//compress expect_str
			if( auto pos = expect_str.find( grammar.T( "0123456789" ) ); pos != StringType::npos )
			{
				expect_str.erase( pos, 10 );
				special_str += grammar.T( "[0-9]" );
			}
			if( auto pos = expect_str.find( grammar.T( "abcdefghijklmnopqrstuvwxyz" ) ); pos != StringType::npos )
			{
				expect_str.erase( pos, 26 );
				special_str += grammar.T( "[a-z]" );
			}
			if( auto pos = expect_str.find( grammar.T( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ) ); pos != StringType::npos )
			{
				expect_str.erase( pos, 26 );
				special_str += grammar.T( "[A-Z]" );
			}
			auto tmp = expect_str;
			expect_str = special_str;
			for( StringElemType ch : tmp )
			{
				if constexpr( std::same_as<char, StringElemType> )
					expect_str += std::format( "{}'{}'", ( expect_str.empty() ? "" : "," ), ch );
				if constexpr( std::same_as<wchar_t, StringElemType> )
					expect_str += std::format( L"{}'{}'", ( expect_str.empty() ? L"" : L"," ), ch );
			}

			if constexpr( std::same_as<char, StringElemType> )
			{
				ret.usr_msg_without_col = std::format( "Syntax Error: unexpected '{}', expect {}.", ret.ch, expect_str );
				ret.usr_msg = std::format( "Syntax Error: unexpected '{}' at column {}, expect {}.", ret.ch, ret.pos, expect_str );
			}
			if constexpr( std::same_as<wchar_t, StringElemType> )
			{
				ret.usr_msg_without_col = std::format( L"Syntax Error: unexpected '{}', expect {}.", ret.ch, expect_str );
				ret.usr_msg = std::format( L"Syntax Error: unexpected '{}' at column {}, expect {}.", ret.ch, ret.pos, expect_str );
			}

			return ret;
		}
		else
			return ErrorInfo{ .type = tError::kNoError };
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

template <grammar_type T = WGrammar<>>
using WCFGparser = CFGparser<T>;

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
inline std::vector<int> string2vector( const std::wstring& s )
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
			for( int i = 1; i < (int)q.size(); ++i )
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
typename Parser::StringType toPostorderExprString( const Parser& cfg, const std::set<int>& targetNT )
{
	auto q = toPostorderSequence( cfg, targetNT );
	typename Parser::StringStreamType ss;
	for( int idx : q )
		ss << cfg.GetGrammar().toString( cfg.GetText( cfg.GetNode( idx ) ) ) << ',';
	return ss.str();
}

}

}
