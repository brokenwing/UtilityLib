#include "pch.h"
#include "../UtilityLib/CFGparser.h"
#include <vector>
#include <map>
#include <functional>

namespace
{
using CFG::CFGtool::string2vector;
}

TEST( Grammar, ok )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();
	int any = 1;
	g.AddNonTerminal( any, { g.toTerminal( 'x' ) } );
	g.AddNonTerminal( any, { g.toTerminal( 'y' ) } );
	g.AddNonTerminal( StartNonTerminal, { any } );
	auto r = g.Initialize();
	EXPECT_EQ( r, Grammar<>::tError::kSuc );
}
TEST( Grammar, missing_terminal )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();
	int any = 1;
	g.AddNonTerminal( StartNonTerminal, { any } );
	auto r = g.Initialize();
	EXPECT_EQ( r, Grammar<>::tError::kNoTerminal );
}
TEST( Grammar, undefinded_nonTerminal )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();
	int any = 1;
	int undef = 2;
	g.AddNonTerminal( any, { g.toTerminal( 'x' ) } );
	g.AddNonTerminal( any, { g.toTerminal( 'y' ) } );
	g.AddNonTerminal( StartNonTerminal, { any,undef } );
	auto r = g.Initialize();
	EXPECT_EQ( r, Grammar<>::tError::kUndefindedNonTerminal );
}
TEST( Grammar, user_type )
{
	using namespace CFG;

	Grammar<int> g;
	g.AddASCIIAsTerminal();
	int any = 1;
	int sth = 2;
	g.AddTerminalList( sth, { 'x','y' }, { .user_data = 1 } );
	g.AddTerminalWord( any, { 'x','y' }, { .user_data = 2 } );
	g.AddNonTerminal( any, { g.toTerminal( 'y' ) }, { .user_data = 3 } );
	g.AddNonTerminal( StartNonTerminal, { any } );
	g.AddNonTerminal( StartNonTerminal, { sth } );

	auto r = g.Initialize();
	EXPECT_EQ( r, Grammar<int>::tError::kSuc );
}

TEST( CFGparser, parse_expr )
{
	using namespace CFG;
	std::map<char, int> c2i;
	c2i['+'] = 10;
	c2i['-'] = 11;
	c2i['*'] = 12;
	c2i['/'] = 13;

	c2i['('] = 20;
	c2i[')'] = 21;
	for( auto i = '0'; i <= '9'; ++i )
		c2i[i] = i - '0';

	Grammar g;
	for( auto& e : c2i )
		g.SetTerminalString( e.second, std::string( 1, e.first ) );
	{//expr of 123+34; 3-4
		const int ope = 1;
		const int digit = 2;
		const int num = 3;
		const int expr = 4;
		const int lb = c2i['('] + g.GetTerminalOffset();
		const int rb = c2i[')'] + g.GetTerminalOffset();
		g.AddTerminalList( digit, { 0,1,2,3,4,5,6,7,8,9 }, { "digit" } );
		g.AddTerminalList( ope, { c2i['+'],c2i['-'],c2i['*'],c2i['/'] }, { "ope" } );
		g.AddNonTerminal( num, { digit,num }, { "num=digit num", false } );
		g.AddNonTerminal( num, { digit }, { "num=digit", false } );
		g.AddNonTerminal( expr, { expr,ope,expr }, { "expr=expr ope expr" } );
		g.AddNonTerminal( expr, { num }, { "expr=num" } );
		g.AddNonTerminal( expr, { lb,expr,rb }, { "expr=(expr)" } );
		g.AddNonTerminal( StartNonTerminal, { expr }, { "S=expr" } );
		auto r = g.Initialize();
		ASSERT_EQ( r, Grammar<>::tError::kSuc );
	}

	auto transform = [&c2i] ( const std::string& s )->std::vector<int>
	{
		std::vector<int> text;
		text.reserve( s.size() );
		for( auto e : s )
			text.emplace_back( c2i[e] );
		return text;
	};
	CFGparser cfg;
	cfg.SetGrammar( g );
	
	EXPECT_TRUE( cfg.Parse( transform( "(1*2)+(3/4)+1" ) ) );
	//std::cout << cfg.toExprTreeString( cfg.GetRoot() ) << '\n';
	EXPECT_TRUE( cfg.Parse( transform( "12345" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "1-2" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "1+2" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "12+23+5+1-2-3-4" ) ) );
	EXPECT_FALSE( cfg.Parse( transform( "+" ) ) );
	EXPECT_FALSE( cfg.Parse( transform( "+2" ) ) );
	EXPECT_FALSE( cfg.Parse( transform( "1-+2" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "(1-2)+3" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "(1)" ) ) );
	EXPECT_FALSE( cfg.Parse( transform( ")3(" ) ) );
}

TEST( CFGgrammarTemplate, parse_number )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();

	const int digit = 1;
	const int num = 2;

	g.SetNonTerminalString( num, "Num" );
	g.SetNonTerminalString( digit, "Digit" );

	GrammarTemplate::AddIntegerDef( g, num, digit );

	g.AddNonTerminal( StartNonTerminal, { num } );

	auto r = g.Initialize();
	ASSERT_EQ( r, Grammar<>::tError::kSuc );

	CFGparser<> cfg;
	cfg.SetGrammar( g );
	
	EXPECT_TRUE( cfg.Parse( string2vector( "1" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "0" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "123" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "100" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "000" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "007" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "0123456789" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "1.2" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "abc" ) ) );
}

TEST( CFGgrammarTemplate, parse_real_number )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();

	const int digit = 1;
	const int num = 2;
	const int realnum = 3;
	
	g.SetNonTerminalString( realnum, "Real" );
	g.SetNonTerminalString( num, "Num" );
	g.SetNonTerminalString( digit, "Digit" );

	GrammarTemplate::AddIntegerDef( g, num, digit );
	GrammarTemplate::AddRealNumberDef( g, realnum, num, g.toTerminal( '.' ) );

	g.AddNonTerminal( StartNonTerminal, { realnum } );

	auto r = g.Initialize();
	ASSERT_EQ( r, Grammar<>::tError::kSuc );

	CFGparser<> cfg;
	cfg.SetGrammar( g );
	
	EXPECT_TRUE( cfg.Parse( string2vector( "1" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "1.2" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "123.456" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "0.1" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( ".1" ) ) );//good?
	EXPECT_FALSE( cfg.Parse( string2vector( "." ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "1.1.1" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "1." ) ) );
}

TEST( CFGgrammarTemplate, parse_scientific_notation )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();

	const int digit = 1;
	const int num = 2;
	const int realnum = 3;
	const int scientific_notation = 4;
	
	g.SetNonTerminalString( realnum, "Real" );
	g.SetNonTerminalString( num, "Num" );
	g.SetNonTerminalString( digit, "Digit" );

	GrammarTemplate::AddIntegerDef( g, num, digit );
	GrammarTemplate::AddRealNumberDef( g, realnum, num, g.toTerminal( '.' ) );
	GrammarTemplate::AddScientificNotationDef( g, scientific_notation, realnum, g.toTerminal( '-' ) );

	g.AddNonTerminal( StartNonTerminal, { scientific_notation } );

	auto r = g.Initialize();
	ASSERT_EQ( r, Grammar<>::tError::kSuc );

	CFGparser<> cfg;
	cfg.SetGrammar( g );
	
	EXPECT_TRUE( cfg.Parse( string2vector( "0e0" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "1E2" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "1.2e2.3" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "1E-2" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "1e-2" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "1.2e-2.3" ) ) );

	EXPECT_FALSE( cfg.Parse( string2vector( "e2" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "1e" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "1e1e" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "1e--2" ) ) );
}

TEST( CFGgrammarTemplate, parse_id )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();
	{
		const int id = 1;
		const int ch = 2;
		const int digit = 3;
		const int num = 4;
		g.SetNonTerminalString( id, "ID" );
		g.SetNonTerminalString( num, "Num" );
		g.SetNonTerminalString( digit, "Digit" );
		g.SetNonTerminalString( ch, "Char" );

		GrammarTemplate::AddIntegerDef( g, num, digit );
		GrammarTemplate::AddIDdef( g, id, ch, num );

		g.AddNonTerminal( StartNonTerminal, { id } );
		auto r = g.Initialize();
		ASSERT_EQ( r, Grammar<>::tError::kSuc );
	}

	CFGparser<> cfg;
	cfg.SetGrammar( g );
	
	EXPECT_TRUE( cfg.Parse( string2vector( "ab" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "ac1" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "aa123ba" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "ad83qgtr83g4t8a83w54" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "a1b2c3d4" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "_" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "a_b" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "_123" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "a b c" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "123" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "123zxc" ) ) );
}

TEST( CFGgrammarTemplate, parse_string )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();

	const int ch = 1;
	const int str = 2;
	const int rawstr = 3;
	const int quote = g.toTerminal( '"' );

	
	g.SetNonTerminalString( ch, "Char" );
	g.SetNonTerminalString( str, "Str" );
	g.SetNonTerminalString( rawstr, "RawStr" );
	g.SetNonTerminalString( quote, "Quote" );

	GrammarTemplate::AddStringDef( g, rawstr, str, quote, quote, ch );

	g.AddNonTerminal( StartNonTerminal, { rawstr } );

	auto r = g.Initialize();
	ASSERT_EQ( r, Grammar<>::tError::kSuc );

	CFGparser<> cfg;
	cfg.SetGrammar( g );
	
	EXPECT_TRUE( cfg.Parse( string2vector( "\"abc\"" ) ) );//"abc"
	EXPECT_TRUE( cfg.Parse( string2vector( "\"x y z\"" ) ) );
	EXPECT_TRUE( cfg.Parse( string2vector( "\"\"" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "abc" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "xx\"xx" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "\"\"\"" ) ) );
	EXPECT_FALSE( cfg.Parse( string2vector( "xx\"xx\"" ) ) );
}

namespace
{
std::pair<bool, std::string> ParseAndPrintPostorderExpr( const std::string& s, bool isPrint = false )
{
	using namespace CFG;
	Grammar g;
	g.AddASCIIAsTerminal();
	
	int digit = 1;
	int num = 2;
	int expr = 3;
	int ope_level_1 = 4;
	int ope_level_2 = 5;
	int ope_level_3 = 9;
	int ope_level_4 = 10;
	int expr_1 = 6;
	int expr_2 = 7;
	int expr_3 = 8;
	int expr_4 = 11;

	g.SetNonTerminalString( num, "Num" );
	g.SetNonTerminalString( digit, "Digit" );
	g.SetNonTerminalString( ope_level_1, "|+,-|" );
	g.SetNonTerminalString( ope_level_2, "|*,/|" );
	g.SetNonTerminalString( ope_level_3, "|^|" );
	g.SetNonTerminalString( ope_level_4, "|-|" );
	g.SetNonTerminalString( expr, "Expr" );
	g.SetNonTerminalString( expr_1, "expr_1" );
	g.SetNonTerminalString( expr_2, "expr_2" );
	g.SetNonTerminalString( expr_3, "expr_3" );
	g.SetNonTerminalString( expr_4, "expr_4" );
	
	g.AddTerminalList( ope_level_1, { '+','-' } );
	g.AddTerminalList( ope_level_2, { '*','/' } );
	g.AddTerminalList( ope_level_3, { '^' } );
	g.AddTerminalList( ope_level_4, { '-' } );

	GrammarTemplate::AddIntegerDef( g, num, digit );
	
	std::vector<GrammarTemplate::ExprPriorityConfig> prioDef;
	prioDef.push_back( { .expr_id = expr_1,.operator_id = ope_level_1,.mark = CFGtool::BinaryOperatorExpandMark } );
	prioDef.push_back( { .expr_id = expr_2,.operator_id = ope_level_2,.mark = CFGtool::BinaryOperatorExpandMark } );
	prioDef.push_back( { .expr_id = expr_3,.operator_id = ope_level_3,.mark = CFGtool::BinaryOperatorExpandMark } );
	prioDef.push_back( { .expr_id = expr_4,.operator_id = ope_level_4,.mark = CFGtool::UnaryOperatorExpandMark,.unary = true } );
	const int main_expr = GrammarTemplate::AddExprWithPriority( g, prioDef, expr, num );

	g.AddNonTerminal( StartNonTerminal, { main_expr } );
	auto r = g.Initialize();
	if( r != Grammar<>::tError::kSuc )
		return { false,"" };
	
	CFGparser<> cfg;
	cfg.SetGrammar( g );
	
	std::set<int> print_target = { num,ope_level_1,ope_level_2,ope_level_3,ope_level_4 };
	
	bool x = cfg.Parse( string2vector( s ) );
	if( !x )
		return { false,"" };
	//std::cout << cfg.toExprTreeString( cfg.GetRoot() ) << '\n';
	return { x,CFGtool::toPostorderExprString( cfg, print_target ) };
}
}

TEST( CFGparser_operator_priority, basic )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1+2" ).second, "1,2,+," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1*2" ).second, "1,2,*," );
}
TEST( CFGparser_operator_priority, long_number )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "12345" ).second, "12345," );
}
TEST( CFGparser_operator_priority, long_number_calc )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "123+321" ).second, "123,321,+," );
}
TEST( CFGparser_operator_priority, same_prio )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1+2-3+4" ).second, "1,2,+,3,-,4,+," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1*2/3*4" ).second, "1,2,*,3,/,4,*," );
}
TEST( CFGparser_operator_priority, prio2_operator2 )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1+2*3" ).second, "1,2,3,*,+," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1*2+3" ).second, "1,2,*,3,+," );
}
TEST( CFGparser_operator_priority, prio2_long_expr )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1*2+3/4" ).second, "1,2,*,3,4,/,+," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1*2+3/4-5*6" ).second, "1,2,*,3,4,/,+,5,6,*,-," );
}
TEST( CFGparser_operator_priority, useless_parentheses )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "(1+2)" ).second, "1,2,+," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "(1*2)" ).second, "1,2,*," );
}
TEST( CFGparser_operator_priority, same_prio_parentheses )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "0-(1+2)" ).second, "0,1,2,+,-," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "0/(1*2)" ).second, "0,1,2,*,/," );
}
TEST( CFGparser_operator_priority, prio2_parentheses )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "(1+2)*3" ).second, "1,2,+,3,*," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "3*(1+2)" ).second, "3,1,2,+,*," );
}
TEST( CFGparser_operator_priority, prio2_parentheses_long )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "(1+2)*(3-4)" ).second, "1,2,+,3,4,-,*," );
}
TEST( CFGparser_operator_priority, prio2_nested_parentheses )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "(3-(1+2))*4" ).second, "3,1,2,+,-,4,*," );
}
TEST( CFGparser_operator_priority, exponentiation )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1^2" ).second, "1,2,^," );
}
TEST( CFGparser_operator_priority, exponentiation_prio_plus )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "0+1^2" ).second, "0,1,2,^,+," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1^2+0" ).second, "1,2,^,0,+," );
}
TEST( CFGparser_operator_priority, exponentiation_prio_multiply )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "0*1^2" ).second, "0,1,2,^,*," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1^2*0" ).second, "1,2,^,0,*," );
}
TEST( CFGparser_operator_priority, exponentiation_parentheses )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "(1+2)^(3+4)" ).second, "1,2,+,3,4,+,^," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "(1*2)^(3/4)" ).second, "1,2,*,3,4,/,^," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1+2^3+4" ).second, "1,2,3,^,+,4,+," );
	EXPECT_EQ( ParseAndPrintPostorderExpr( "1*2^3/4" ).second, "1,2,3,^,*,4,/," );
}
TEST( CFGparser_operator_priority, neg )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "-1" ).second, "1,-," );
}
TEST( CFGparser_operator_priority, neg_plus )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "-1+2" ).second, "1,-,2,+," );
}
TEST( CFGparser_operator_priority, neg_multi )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "--1" ).second, "1,-,-," );
}
TEST( CFGparser_operator_priority, neg_multiply )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "-1*-2" ).second, "1,-,2,-,*," );
}
TEST( CFGparser_operator_priority, neg_exponentiation )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "-1^-2" ).second, "1,-,2,-,^," );
}
TEST( CFGparser_operator_priority, neg_exponentiation_parentheses )
{
	EXPECT_EQ( ParseAndPrintPostorderExpr( "-(1^-2)" ).second, "1,2,-,^,-," );
}

namespace
{
std::string ParseAndPrintPosasdtorderOFVexpr( const std::string& s )
{
	using namespace CFG;

	enum struct tNT
	{
		kStart = StartNonTerminal,
		kID, kChar,
		kScientificNotation, kReal, kInt, kDigit,
		kSpace, kMultiSpace,
		kUnit, kExpr, kFunc, kParam,
		kSubExpr1, kSubExpr2, kSubExpr3, kSubExpr4,
		kOpGroup1, kOpGroup2, kOpGroup3, kOpGroup4,
		kOther,
	};
	Grammar<> g;
	g.AddASCIIAsTerminal();

	g.SetNonTerminalString( (int)tNT::kID, "ID" );
	g.SetNonTerminalString( (int)tNT::kInt, "Int" );
	g.SetNonTerminalString( (int)tNT::kReal, "Real" );
	g.SetNonTerminalString( (int)tNT::kDigit, "Digit" );
	g.SetNonTerminalString( (int)tNT::kChar, "Char" );//for id
	g.SetNonTerminalString( (int)tNT::kSpace, "Space" );
	g.SetNonTerminalString( (int)tNT::kMultiSpace, "MultiSpace" );
	g.SetNonTerminalString( (int)tNT::kUnit, "Unit" );
	g.SetNonTerminalString( (int)tNT::kExpr, "Expr" );
	g.SetNonTerminalString( (int)tNT::kFunc, "Func" );
	g.SetNonTerminalString( (int)tNT::kParam, "Param" );
	g.SetNonTerminalString( (int)tNT::kSubExpr1, "SubExpr1" );
	g.SetNonTerminalString( (int)tNT::kSubExpr2, "SubExpr2" );
	g.SetNonTerminalString( (int)tNT::kSubExpr3, "SubExpr3" );
	g.SetNonTerminalString( (int)tNT::kSubExpr4, "SubExpr4" );
	g.SetNonTerminalString( (int)tNT::kOpGroup1, "Op|+,-|" );
	g.SetNonTerminalString( (int)tNT::kOpGroup2, "Op|*,/|" );
	g.SetNonTerminalString( (int)tNT::kOpGroup3, "Op|^|" );
	g.SetNonTerminalString( (int)tNT::kOpGroup4, "Op|-|" );

	g.AddTerminalList( (int)tNT::kOpGroup1, { '+','-' } );
	g.AddTerminalList( (int)tNT::kOpGroup2, { '*','/' } );
	g.AddTerminalList( (int)tNT::kOpGroup3, { '^' } );
	g.AddTerminalList( (int)tNT::kOpGroup4, { '-' } );

	GrammarTemplate::AddIntegerDef( g, (int)tNT::kInt, (int)tNT::kDigit );
	GrammarTemplate::AddRealNumberDef( g, (int)tNT::kReal, (int)tNT::kInt, g.toTerminal( '.' ) );
	GrammarTemplate::AddScientificNotationDef( g, (int)tNT::kScientificNotation, (int)tNT::kReal, g.toTerminal( '-' ) );
	GrammarTemplate::AddIDdef( g, (int)tNT::kID, (int)tNT::kChar, (int)tNT::kInt );

	g.AddTerminalList( (int)tNT::kSpace, { ' ','\t' } );
	g.AddNonTerminal( (int)tNT::kMultiSpace, { (int)tNT::kSpace,(int)tNT::kMultiSpace } );
	g.AddNonTerminal( (int)tNT::kMultiSpace, { (int)tNT::kSpace } );

	g.AddNonTerminal( (int)tNT::kUnit, { (int)tNT::kReal } );
	g.AddNonTerminal( (int)tNT::kUnit, { (int)tNT::kScientificNotation } );
	g.AddNonTerminal( (int)tNT::kUnit, { (int)tNT::kFunc }, { .mark = CFGtool::ForceExpandMark } );
	g.AddNonTerminal( (int)tNT::kUnit, { g.toTerminal( '[' ), (int)tNT::kInt,g.toTerminal( ']' ) } );//[idx]  user prop
	g.AddNonTerminal( (int)tNT::kUnit, { g.toTerminal( '[' ), (int)tNT::kID,g.toTerminal( ']' ) } );//[keyword]

	g.AddNonTerminal( (int)tNT::kFunc, { (int)tNT::kID,g.toTerminal( '(' ),(int)tNT::kParam,g.toTerminal( ')' ) }, { .mark = CFGtool::FunctionExpandMark } );

	std::vector<GrammarTemplate::ExprPriorityConfig> prioDef;
	prioDef.push_back( { .expr_id = (int)tNT::kSubExpr1,.operator_id = (int)tNT::kOpGroup1,.mark = CFGtool::BinaryOperatorExpandMark } );
	prioDef.push_back( { .expr_id = (int)tNT::kSubExpr2,.operator_id = (int)tNT::kOpGroup2,.mark = CFGtool::BinaryOperatorExpandMark } );
	prioDef.push_back( { .expr_id = (int)tNT::kSubExpr3,.operator_id = (int)tNT::kOpGroup3,.mark = CFGtool::BinaryOperatorExpandMark } );
	prioDef.push_back( { .expr_id = (int)tNT::kSubExpr4,.operator_id = (int)tNT::kOpGroup4,.mark = CFGtool::UnaryOperatorExpandMark,.unary = true } );
	const int main_expr = GrammarTemplate::AddExprWithPriority( g, prioDef, (int)tNT::kExpr, (int)tNT::kUnit );
	
	g.AddNonTerminal( (int)tNT::kParam, { main_expr } );
	g.AddNonTerminal( (int)tNT::kParam, { main_expr,g.toTerminal( ',' ),(int)tNT::kParam } );

	g.AddNonTerminal( StartNonTerminal, { main_expr } );

	auto r = g.Initialize();
	if( r != Grammar<>::tError::kSuc )
		return "";
	CFGparser<> cfg;
	cfg.SetGrammar( g );

	std::set<int> target = { (int)tNT::kUnit,(int)tNT::kID,(int)tNT::kOpGroup1,(int)tNT::kOpGroup2,(int)tNT::kOpGroup3,(int)tNT::kOpGroup4 };
	cfg.Parse( string2vector( s ) );
	return CFGtool::toPostorderExprString( cfg, target );
}
}

TEST( CFGsample, unit )
{
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "[0]" ), "[0]," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "[est]" ), "[est]," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "1" ), "1," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "1e2" ), "1e2," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "1.5" ), "1.5," );
}
TEST( CFGsample, NGunit )
{
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "[123x]" ), "" );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "-" ), "" );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "()" ), "" );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "abc" ), "" );
}
TEST( CFGsample, func )
{
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "func(0)" ), "0,func," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "func(0,1)" ), "0,1,func," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "func(1+2)" ), "1,2,+,func," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "func(1+2,3)" ), "1,2,+,3,func," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "func([0])" ), "[0],func," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "func(1,2,3,4)" ), "1,2,3,4,func," );
}
TEST( CFGsample, NGfunc )
{
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "func()" ), "" );//empty function is no use here
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "f(,)" ), "" );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "f(1,)" ), "" );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "f(" ), "" );
}
TEST( CFGsample, nestedfunc )
{
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "f1(f2([x]))" ), "[x],f2,f1," );
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "f1(f2(0),f3(1)+f4(2))" ), "0,f2,1,f3,2,f4,+,f1," );
}
TEST( CFGsample, realcase )
{
	EXPECT_EQ( ParseAndPrintPosasdtorderOFVexpr( "[0]*max(0,[t3]-[let])^2+[setup]*[1]" ), "[0],0,[t3],[let],-,max,2,^,*,[setup],[1],*,+," );
}