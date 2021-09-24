#pragma once
#include "pch.h"
#include "CommonDef.h"
#include <vector>

namespace Util
{
template <typename T,
	typename RowContainer = std::vector<T, DefaultAllocator<T>>,
	typename Container = std::vector<RowContainer, DefaultAllocator<RowContainer> >>
class Matrix
{
public:
	using value_type = typename T;

protected:
	Container data;

public:
	Matrix()
	{}
	Matrix( size_t n, size_t m, const T val = T() )
	{
		resize( n, m, val );
	}
	Matrix( std::initializer_list<std::initializer_list<T>>&& mat )
	{
		size_t n = mat.size();
		size_t m = 0;
		for( auto& e : mat )
			m = std::max( m, e.size() );

		data.reserve( n );
		for( auto& row : mat )
		{
			auto& cur = data.emplace_back();
			cur.resize( m );
			int idx = 0;
			for( auto& e : row )
				cur[idx++] = e;
		}
		assert( size() == std::make_pair( n, m ) );
	}

	//only number
	static Matrix Diagonal( size_t n )
	{
		Matrix mat( n, n, 0 );
		for( int i = 0; i < (int)n; i++ )
			mat.data[i][i] = 1;
		return mat;
	}

	const typename Container::value_type& operator[]( int idx ) const	{		return data[idx];	}
		  typename Container::value_type& operator[]( int idx )			{		return data[idx];	}
	const T& at( int x, int y )const	{		return data[x][y];	}
		  T& at( int x, int y )			{		return data[x][y];	}

	void swap( Matrix& other )	{		data.swap( other.data );	}
	void fill( const T& val )
	{
		for( auto& row : data )
			for( auto& col : row )
				col = val;
	}
	void clear()	{		data.clear();	}
	void resize( size_t n, size_t m, const T val = T() )
	{
		data.clear();
		data.resize( n );
		for( auto& e : data )
			e.resize( m, val );
	}
	bool empty()const	{		return data.empty();	}
	std::pair<std::size_t, std::size_t> size()const
	{
		assert( selfcheck() );
		if( data.empty() )
			return std::make_pair( 0, 0 );
		return std::make_pair( data.size(), data.front().size() );
	}
	void Transpose()
	{
		auto len = size();
		if( len.first == len.second )
		{
			for( int i = 0; i < (int)len.first; i++ )
				for( int j = i + 1; j < (int)len.second; j++ )
					std::swap( data[i][j], data[j][i] );
		}
		else
		{
			Matrix tmp( len.second, len.first );
			for( int i = 0; i < (int)len.first; i++ )
				for( int j = 0; j < (int)len.second; j++ )
					tmp.data[j][i] = data[i][j];
			this->swap( tmp );
		}
	}
	//Matrix multiplication
	Matrix MatMul( const Matrix& other )const
	{
		auto la = size();
		auto lb = other.size();
		assert( la.second == lb.first );
		Matrix c( la.first, lb.second );
		for( int i = 0; i < (int)la.first; i++ )
			for( int j = 0; j < (int)lb.second; j++ )
				for( int k = 0; k < (int)la.second; k++ )
					c[i][j] += data[i][k] * other.data[k][j];
		return c;
	}

	//operation with matrix

	bool operator==( const Matrix& other )const
	{
		if( size() != other.size() )
			return false;
		auto l = size();
		for( int i = 0; i < (int)l.first; ++i )
			for( int j = 0; j < (int)l.second; ++j )
				if( !( data[i][j] == other.data[i][j] ) )
					return false;
		return true;
	}
	Matrix operator+( const Matrix& other )const
	{
		Matrix c = *this;
		Matrix::Apply( *this, other, c, [] ( const T& a, const T& b )->T
		{
			return a + b;
		} );
		return c;
	}
	Matrix operator-( const Matrix& other )const
	{
		Matrix c = *this;
		Matrix::Apply( *this, other, c, [] ( const T& a, const T& b )->T
		{
			return a - b;
		} );
		return c;
	}
	Matrix operator*( const Matrix& other )const
	{
		Matrix c = *this;
		Matrix::Apply( *this, other, c, [] ( const T& a, const T& b )->T
		{
			return a * b;
		} );
		return c;
	}
	Matrix operator/( const Matrix& other )const
	{
		Matrix c = *this;
		Matrix::Apply( *this, other, c, [] ( const T& a, const T& b )->T
		{
			return a / b;
		} );
		return c;
	}

	//operation with number

	template <typename T_Num>
	Matrix operator+( const T_Num& val )const
	{
		Matrix c = *this;
		c.Apply( [&val] ( T& x )->void
		{
			x += val;
		} );
		return c;
	}
	template <typename T_Num>
	Matrix operator-( const T_Num& val )const
	{
		Matrix c = *this;
		c.Apply( [&val] ( T& x )->void
		{
			x -= val;
		} );
		return c;
	}
	template <typename T_Num>
	Matrix operator*( const T_Num& val )const
	{
		Matrix c = *this;
		c.Apply( [&val] ( T& x )->void
		{
			x *= val;
		} );
		return c;
	}
	template <typename T_Num>
	Matrix operator/( const T_Num& val )const
	{
		Matrix c = *this;
		c.Apply( [&val] ( T& x )->void
		{
			x /= val;
		} );
		return c;
	}

protected:
	bool selfcheck() const
	{
		std::set<std::size_t> len;
		for( auto& e : data )
			len.insert( e.size() );
		return len.size() <= 1;
	}

private:
	template<typename Func>
	void Apply( Func func )//Apply function to each element
	{
		for( auto& row : data )
			for( auto& e : row )
				func( e );
	}

	template<typename Func>
	static void Apply( const Matrix& a, const Matrix& b, Matrix& ret, Func func )//for each element: func(a,b)->ret
	{
		assert( a.size() == b.size() );
		assert( a.size() == ret.size() );
		auto l = ret.size();
		for( int i = 0; i < (int)l.first; ++i )
			for( int j = 0; j < (int)l.second; ++j )
				ret.data[i][j] = func( a.data[i][j], b.data[i][j] );
	}
};
}