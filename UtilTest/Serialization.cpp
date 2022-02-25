#include "pch.h"
#include <array>
#include "Serialization.h"

using namespace Util::Serialization;

struct SmallSerialization :public serialization
{
	int x = 3;

	// Inherited via Serialization
	virtual int serialize( ostream_type& out ) const override
	{
		serialization::serialize_basic_type<int>( x, out );
		return 0;
	}
	virtual int unserialize( istream_type& in ) override
	{
		serialization::unserialize_basic_type<int>( x, in );
		return 0;
	}
};
struct SampleSerialization:public serialization
{
	struct Point
	{
		int x = 5;
		int y = 6;
	};

	int a = 1;
	double b = 2.3;
	bool c = true;
	std::string s1 = "123";
	std::wstring s2 = L"ÊÇ";
	Point pt;
	SmallSerialization small;

	void clear()
	{
		a = 0;
		b = 0;
		c = false;
		s1 = "";
		s2 = L"";
		pt.x = 0;
		pt.y = 0;
		small.x = 0;
	}

	// Inherited via Serialization
	virtual int serialize( ostream_type& out ) const override
	{
		serialization::serialize_basic_type<int>( a, out );
		serialization::serialize_basic_type<double>( b, out );
		serialization::serialize_basic_type<bool>( c, out );
		serialization::serialize_string( s1, out );
		serialization::serialize_string( s2, out );
		serialization::serialize_entity( pt, out );
		small.serialize( out );
		return 0;
	}
	virtual int unserialize( istream_type& in ) override
	{
		serialization::unserialize_basic_type<int>( a, in );
		serialization::unserialize_basic_type<double>( b, in );
		serialization::unserialize_basic_type<bool>( c, in );
		serialization::unserialize_string( s1, in );
		serialization::unserialize_string( s2, in );
		serialization::unserialize_entity( pt, in );
		small.unserialize( in );
		return 0;
	}
};

TEST( Serialization, fstream )
{
	SampleSerialization from, to;
	to.clear();

	std::ofstream out;
	out.open( "tmp.dat", std::ios::binary );
	from.serialize( out );
	out.close();

	std::ifstream in( "tmp.dat", std::ios::binary );
	to.unserialize( in );
	in.close();
	
	EXPECT_EQ( from.a, to.a );
	EXPECT_EQ( from.b, to.b );
	EXPECT_EQ( from.c, to.c );
	EXPECT_EQ( from.s1, to.s1 );
	EXPECT_EQ( from.s2, to.s2 );
}

TEST( Serialization, stringstream )
{
	SampleSerialization from, to;
	to.clear();

	std::ostringstream out;
	from.serialize( out );

	auto s = out.str();
	std::istringstream in( s );
	to.unserialize( in );

	EXPECT_EQ( from.a, to.a );
	EXPECT_EQ( from.b, to.b );
	EXPECT_EQ( from.c, to.c );
	EXPECT_EQ( from.s1, to.s1 );
	EXPECT_EQ( from.s2, to.s2 );
}

TEST( Serialization, recursive_vector )
{
	std::vector<int> from = { 1,2,3 }, to;

	std::ostringstream out;
	serialization::serialize_recursively( from, out );

	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively( to, in );

	EXPECT_EQ( from, to );
}
TEST( Serialization, recursive_vector_pair )
{
	std::vector<std::pair<int, double>> from = { {1,1.1},{2,2.2} }, to;

	std::ostringstream out;
	serialization::serialize_recursively( from, out );

	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively( to, in );

	EXPECT_EQ( from, to );
}
TEST( Serialization, recursive_vector_pair_string )
{
	std::array<char, 3> s1 = { 'a','b','c' }, s2 = { 'x','x' };
	std::vector<std::pair<std::array<char, 3>, std::string>> from = { {s1,std::string( "xy" )},{s2,std::string( "yyy" )} }, to;

	std::ostringstream out;
	serialization::serialize_recursively( from, out );

	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively( to, in );

	EXPECT_EQ( from, to );
}
TEST( Serialization, recursive_wstring )
{
	std::wstring from = L"xy", to;

	std::ostringstream out;
	serialization::serialize_recursively( from, out );

	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively( to, in );

	EXPECT_EQ( from, to );
}
TEST( Serialization, recursive_wchar )
{
	wchar_t from[10] = L"xy", to[10];
	
	std::ostringstream out;
	serialization::serialize_recursively( from, out );

	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively( to, in );

	EXPECT_EQ( wcscmp( from, to ), 0 );
}
TEST( Serialization, recursive_char )
{
	char from[10] = "xy", to[10];

	std::ostringstream out;
	serialization::serialize_recursively( from, out );

	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively( to, in );

	EXPECT_EQ( strcmp( from, to ), 0 );
}
TEST( Serialization, recursive_array )
{
	std::array<int, 4> from = { 1,2,3,4 }, to;

	std::ostringstream out;
	serialization::serialize_recursively( from, out );

	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively( to, in );

	EXPECT_EQ( from, to );
}
TEST( Serialization, recursive_tuple )
{
	std::tuple<int, double, char> from = { 0,1.2,3 }, to;

	std::ostringstream out;
	serialization::serialize_recursively( from, out );

	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively( to, in );

	EXPECT_EQ( from, to );
}

struct ComplexEntity
{
	int a = 1;
	char b = 'a';
	double c = 2.3;
	std::string s = "asd";

	void clear()
	{
		a = 0;
		b = '\0';
		c = 0;
		s = "";
	}
	AddComplexEntitySerialization( a, b, c, s )
};
TEST( Serialization, recursive_any_entity )
{
	ComplexEntity from, to;
	to.clear();
	
	std::ostringstream out;
	serialization::serialize_recursively(from, out );
	
	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively(to, in );
	
	EXPECT_EQ( from.a, to.a );
	EXPECT_EQ( from.b, to.b );
	EXPECT_EQ( from.c, to.c );
	EXPECT_EQ( from.s, to.s );
}

TEST( Serialization, recursive_simple_entity )
{
	struct Simple
	{
		int a = -1;
		double b = 1.2;
		AddSimpleEntitySerialization
	};
	Simple from, to;
	to.a = 0;
	to.b = 0;
	
	std::ostringstream out;
	serialization::serialize_recursively(from, out );
	
	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively(to, in );
	
	EXPECT_EQ( from.a, to.a );
	EXPECT_EQ( from.b, to.b );
}

TEST( Serialization, recursive_inherit )
{
	struct A
	{
		int a = -1;
		AddSimpleEntitySerialization
	};
	struct B :public A
	{
		int b = 1;
	};
	B from, to;
	to.a = 0;
	to.b = 0;
	
	std::ostringstream out;
	serialization::serialize_recursively(from, out );
	
	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively(to, in );
	
	EXPECT_EQ( from.a, to.a );
	EXPECT_EQ( from.b, to.b );
}
TEST( Serialization, recursive_part_inherit )
{
	struct A
	{
		int a = -1;
		AddSimpleEntitySerialization
	};
	struct B :public A
	{
		int b = 1;
	};
	B from, to;
	to.a = 0;
	to.b = 0;
	
	std::ostringstream out;
	serialization::serialize_recursively<A>(from, out );
	
	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively<A>(to, in );
	
	EXPECT_EQ( from.a, to.a );
	EXPECT_EQ( 0, to.b );
}
TEST( Serialization, recursive_part_inherit_B )
{
	struct A
	{
		int a = -1;
	};
	struct B :public A
	{
		int b = 1;
		AddComplexEntitySerialization(b)
	};
	B from, to;
	to.a = 0;
	to.b = 0;
	
	std::ostringstream out;
	serialization::serialize_recursively(from, out );
	
	auto s = out.str();
	std::istringstream in( s );
	serialization::unserialize_recursively(to, in );
	
	EXPECT_EQ( from.b, to.b );
	EXPECT_EQ( 0, to.a );
}