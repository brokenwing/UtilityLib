#include "pch.h"
#include "MultipleIndexing.h"
#include <array>

using namespace Util::Indexing;

struct IntElement :ElementInterface
{
	int me = 0;
};
struct HybridElement :ElementInterface
{
	double me = 0;

	template <typename T>
	T GetKey( const int id )const
	{
		return T();
	}
	template <typename T>
	T GetValue( const int id )const
	{
		return T();
	}

	template <>
	int GetKey( const int id )const
	{
		return static_cast<int>( me );
	}
	template <>
	int GetValue( const int id )const
	{
		return static_cast<int>( me );
	}
	template <>
	double GetKey( const int id )const
	{
		return me;
	}
	template <>
	double GetValue( const int id )const
	{
		return me;
	}
};

TEST( MultipleIndexing, compile_int )
{
	using MyIndex = BaseIndexing<IntElement>;

	MultipleIndexing<ArrayIndexing<MyIndex>, OrderedIndexing<MyIndex>, HashIndexing<MyIndex>> mti;
	{
		auto& me = std::get<0>( mti );
		me.clear();
		me.insert( IntElement() );
		me.erase( IntElement() );
		me.find( IntElement() );
		me.contain( IntElement() );
		const auto& fix = me;
		fix.find( IntElement() );
	}
	{
		auto& me = std::get<1>( mti );
		me.clear();
		me.insert( IntElement() );
		me.erase( IntElement() );
		me.find( IntElement() );
		me.contain( IntElement() );
		const auto& fix = me;
		fix.find( IntElement() );
	}
	{
		auto& me = std::get<2>( mti );
		me.clear();
		me.insert( IntElement() );
		me.erase( IntElement() );
		me.find( IntElement() );
		me.contain( IntElement() );
		const auto& fix = me;
		fix.find( IntElement() );
	}
	std::vector<int> q;
	mti.for_each( [&] ( auto& me )
	{
		q.emplace_back( me.id );
	} );
	EXPECT_EQ( q.size(), mti.size() );

	mti.clear();
	mti.insert( IntElement() );
	mti.erase( IntElement() );
	mti.at<0>().name = "";
}

TEST( MultipleIndexing, compile_hybrid )
{
	using MyIndex = BaseIndexing<HybridElement>;
	
	MultipleIndexing<ArrayIndexing<MyIndex, HybridElement>, OrderedIndexing<MyIndex, double, HybridElement>, HashIndexing<MyIndex, int, double>> mti;
	{
		auto& me = std::get<0>( mti );
		me.clear();
		me.insert( HybridElement() );
		me.erase( HybridElement() );
		me.find( HybridElement() );
		me.contain( HybridElement() );
		const auto& fix = me;
		fix.find( HybridElement() );
	}
	{
		auto& me = std::get<1>( mti );
		me.clear();
		me.insert( HybridElement() );
		me.erase( HybridElement() );
		me.find( HybridElement() );
		me.contain( HybridElement() );
		const auto& fix = me;
		fix.find( HybridElement() );
	}
	{
		auto& me = std::get<2>( mti );
		me.clear();
		me.insert( HybridElement() );
		me.erase( HybridElement() );
		me.find( HybridElement() );
		me.contain( HybridElement() );
		const auto& fix = me;
		fix.find( HybridElement() );
	}
}

struct MyElement :ElementInterface
{
	int x, y;
	double w;

	template <typename T>
	T GetKey( const int id )const
	{
		return T();
	}
	template <typename T>
	T GetValue( const int id )const
	{
		return T();
	}

	template <>
	std::pair<int, int> GetKey( const int id )const
	{
		return { x,y };
	}
	template <>
	int GetKey( const int id )const
	{
		return x * 10 + y;
	}
	template <>
	double GetKey( const int id )const
	{
		return w;
	}
	template <>
	double GetValue( const int id )const
	{
		return w;
	}
	template <>
	MyElement GetValue( const int id )const
	{
		return *this;
	}
};
TEST( MultipleIndexing, compile_diy )
{
	const int N = 10;

	
	using MyIndex = BaseIndexing<MyElement>;

	struct MyMap :public MyIndex
	{
		using value_type = MyIndex::value_type;
		using key_type = std::pair<int, int>;
		using data_type = double;

		std::array<std::array<double, N>, N> data;

		data_type find( const value_type& elm )
		{
			auto pos = elm.GetKey<key_type>( MyIndex::id );
			return data[pos.first][pos.second];
		}
		void insert( const value_type& elm )
		{
			auto pos = elm.GetKey<key_type>( MyIndex::id );
			auto val = elm.GetValue<data_type>( MyIndex::id );
			data[pos.first][pos.second] = val;
		}
		void erase( const value_type& elm )
		{
			auto pos = elm.GetKey<key_type>( MyIndex::id );
			data[pos.first][pos.second] = 0;
		}
		bool contain( const value_type& elm )const
		{
			return true;
		}
		void clear()
		{
			for( auto& e : data )
				e.fill( 0 );
		}
	};

	MultipleIndexing<ArrayIndexing<MyIndex, MyElement>, OrderedIndexing<MyIndex, double, MyElement>, MyMap> mti;
	constexpr int Idx2Elm = 0;
	constexpr int W2Elm = 1;
	constexpr int Pos2Elm = 2;

	MyElement a{ .x = 1,.y = 1,.w = 99 };
	MyElement b{ .x = 1,.y = 2,.w = 5 };
	MyElement c{ .x = 1,.y = 3,.w = 55 };

	mti.insert( a );
	mti.insert( b );
	mti.insert( c );

	auto bb = mti.at<W2Elm>().first();
	EXPECT_EQ( bb.x, b.x );
	EXPECT_EQ( bb.y, b.y );
	EXPECT_DOUBLE_EQ( bb.w, b.w );

	auto cw = mti.at<Pos2Elm>().data[1][3];
	EXPECT_DOUBLE_EQ( cw, c.w );
}
