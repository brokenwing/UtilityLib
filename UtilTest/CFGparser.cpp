#include <pch.h>
#include "../UtilityLib/CFGparser.h"
#include <vector>
#include <map>

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
		g.AddTerminalList( digit, { 0,1,2,3,4,5,6,7,8,9 }, "digit" );
		g.AddTerminalList( ope, { c2i['+'],c2i['-'],c2i['*'],c2i['/'] }, "ope" );
		g.AddNonTerminal( num, { digit,num }, "num=digit num", false );
		g.AddNonTerminal( num, { digit }, "num=digit", false );
		g.AddNonTerminal( expr, { expr,ope,expr }, "expr=expr ope expr" );
		g.AddNonTerminal( expr, { num }, "expr=num" );
		g.AddNonTerminal( expr, { lb,expr,rb }, "expr=(expr)" );
		g.AddNonTerminal( StartNonTerminal, { expr }, "S=expr" );
		g.Initialize();
	}

	auto transform = [&] ( const std::string &s )->std::vector<int>
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

TEST( CFGparser, parse_id )
{
	using namespace CFG;

	Grammar g;
	for( int i = 1; i < 128; i++ )
		g.SetTerminalString( i, std::string( 1, char( i ) ) );
	{
		const int id = 1;
		const int ch = 2;
		const int digit = 3;
		const int num = 4;
		const int seperator = 5;
		const int seperators = 6;
		const int text = 7;
		g.SetNonTerminalString( id, "ID" );
		g.SetNonTerminalString( num, "Num" );
		g.SetNonTerminalString( digit, "Digit" );
		g.SetNonTerminalString( ch, "Char" );
		g.SetNonTerminalString( seperator, "Space" );
		g.SetNonTerminalString( seperators, "ManySpace" );
		g.SetNonTerminalString( text, "Text" );

		g.AddTerminalList( digit, { '0','1','2','3','4','5','6','7','8','9' }, "digit" );
		g.AddNonTerminal( num, { digit,num } );
		g.AddNonTerminal( num, { digit } );
		for( char c = 'a'; c <= 'z'; ++c )
			g.AddTerminalList( ch, { c }, "char" );
		for( char c = 'A'; c <= 'Z'; ++c )
			g.AddTerminalList( ch, { c }, "char" );
		g.AddTerminalList( ch, { '_' }, "char" );
		
		g.AddNonTerminal( id, { ch } );
		g.AddNonTerminal( id, { ch,id } );
		g.AddNonTerminal( id, { ch,num } );
		g.AddNonTerminal( id, { ch,num,id} );
		
		g.AddTerminalList( seperator, { ' ','\n','\t' }, "char" );
		g.AddNonTerminal( seperators, { seperator,seperators });
		g.AddNonTerminal( seperators, { seperator } );

		g.AddNonTerminal( text, { text,seperators,text } );
		g.AddNonTerminal( text, { id } );
		g.AddNonTerminal( StartNonTerminal, { text }, "S=expr" );
		g.Initialize();
	}

	auto transform = [&] ( const std::string &s )->std::vector<int>
	{
		std::vector<int> text;
		text.reserve( s.size() );
		for( auto e : s )
			text.emplace_back( e );
		return text;
	};
	CFGparser cfg;
	cfg.SetGrammar( g );
	
	EXPECT_TRUE( cfg.Parse( transform( "ab" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "ac1" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "aa123ba" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "ad83qgtr83g4t8a83w54" ) ) );
	EXPECT_TRUE( cfg.Parse( transform( "aasd asd  d45" ) ) );
	//std::cout << cfg.toExprTreeString( cfg.GetRoot() ) << '\n';
	EXPECT_FALSE( cfg.Parse( transform( "123" ) ) );
	EXPECT_FALSE( cfg.Parse( transform( "123zxc" ) ) );
}
