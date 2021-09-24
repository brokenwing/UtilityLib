#pragma once
#include "CommonDef.h"
#include "VecUtil.h"

namespace Util
{
//Internal
template<unsigned int _RADIX>
class _LongInt
{
protected:
	typedef unsigned long long ULL;
	static constexpr unsigned int RADIX = _RADIX;

	bool sign = true;//postive
	int n = 1;//有效长度
	LFA::vector<unsigned int> m_val;

public:
	_LongInt()
	{
		m_val.resize( 1, 0 );
	}
	_LongInt( int val )
	{
		m_val.resize( 1, abs(val) );
		sign = val >= 0;
	}
	_LongInt( const LFA::string& s, char delimiter = ' ' )
	{
		if( s.empty() )
		{
			m_val.resize( 1, 0 );
			return;
		}
		sign = ( s[0] != '-' );
		const int first = sign ? 0 : 1;

		n = 1;
		for( char c : s )
			n += ( c == delimiter );
		m_val.reserve( n );
		int offset = (int)s.length();
		while( offset >= 0 )
		{
			const int pos = (int)s.rfind( delimiter, offset );
			unsigned int v = 0;
			sscanf_s( s.data() + ( pos == -1 ? first : pos ), "%u", &v );
			m_val.emplace_back( v % RADIX );
			if( pos == -1 )
				break;
			offset = pos - 1;
		}
	}
	~_LongInt()
	{}

protected:
	bool check()const
	{
		for( int i = 0; i < n; i++ )
			if( m_val[i] < 0 || m_val[i] >= RADIX )
				return false;
		return true;
	}
	int count_n(int from)const
	{
		for( int i = from - 1; i >= 0; i-- )
			if( m_val[i] > 0 )
				return i + 1;
		return 1;
	}
	bool isZero()const
	{
		return n == 1 && m_val[0] == 0;
	}
	//ignroe sign

	bool operator<( const _LongInt& other )const
	{
		if( n != other.n )
			return n < other.n;
		else
		{
			return LT( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
		}
	}
	bool operator>( const _LongInt& other )const
	{
		if( n != other.n )
			return n > other.n;
		else
		{
			return GT( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
		}
	}
	bool operator<=( const _LongInt& other )const
	{
		return !( *this > other );
	}
	bool operator>=( const _LongInt& other )const
	{
		return !( *this < other );
	}
	bool operator==( const _LongInt& other )const
	{
		return n == other.n && EQ( this->m_val.rbegin(), other.m_val.rbegin(), n );
	}
	bool operator!=( const _LongInt& other )const
	{
		return !( *this == other );
	}

	static void UnsignedPlus( const _LongInt & a, const _LongInt & b, _LongInt & c )
	{
		const _LongInt* n_long = ( a.n > b.n ) ? &a : &b;
		const _LongInt* n_short = ( a.n > b.n ) ? &b : &a;
		c.m_val.resize( (size_t)n_long->n + 1, 0 );
		c.n = n_long->n;

		unsigned int offset = 0;
		auto it_long = n_long->m_val.cbegin();
		auto it_short = n_short->m_val.cbegin();
		auto it = c.m_val.begin();
		for( int i = 0; i < n_short->n; i++, ++it, ++it_long, ++it_short )
		{
			unsigned int tmp = offset + *it_long + *it_short;//no overflow
			*it = tmp % RADIX;
			offset = tmp / RADIX;
		}
		for( int i = n_short->n; i < n_long->n; i++, ++it, ++it_long )
		{
			unsigned int tmp = offset + *it_long;
			*it = tmp % RADIX;
			offset = tmp / RADIX;
		}
		if( offset != 0 )
		{
			*it = offset;
			c.n++;
		}
		assert( c.check() );
	}
	
	static void UnsignedMinus( const _LongInt& a, const _LongInt& b, _LongInt& c )
	{
		assert( a.n >= b.n );
		assert( a >= b );
		const _LongInt* n_long = &a;
		const _LongInt* n_short = &b;
		c.m_val.resize( n_long->n, 0 );
		c.n = n_long->n;

		int offset = 0;
		auto it_long = n_long->m_val.cbegin();
		auto it_short = n_short->m_val.cbegin();
		auto it = c.m_val.begin();
		for( int i = 0; i < n_short->n; i++, ++it, ++it_long, ++it_short )
		{
			int tmp = offset + *it_long - *it_short;//no overflow
			offset = -( tmp < 0 );
			tmp += RADIX * ( tmp < 0 );
			assert( tmp >= 0 );
			*it = tmp;
		}
		for( int i = n_short->n; i < n_long->n; i++, ++it, ++it_long )
		{
			int tmp = offset + *it_long;//no overflow
			offset = -( tmp < 0 );
			tmp += RADIX * ( tmp < 0 );
			assert( tmp >= 0 );
			*it = tmp;
		}
		c.n = c.count_n( c.n );
		assert( c.check() );
	}

	static void SignedPlus( const _LongInt& a, const _LongInt& b, bool sign_a, bool sign_b, _LongInt& c )
	{
		if( sign_a == sign_b )
		{
			UnsignedPlus( a, b, c );
			c.sign = sign_a;
		}
		else
		{
			const bool ge = a >= b;
			if( ge )
				UnsignedMinus( a, b, c );
			else
				UnsignedMinus( b, a, c );
			//a b   c
			//+ - > +
			//+ - < -
			//- + > -
			//- + < +
			c.sign = ge ^ sign_b;
		}
	}
};


//RADIX = 10^8
class LongInt :private _LongInt<100000000>
{
private:

public:
	~LongInt()
	{}
	LongInt() :_LongInt()
	{}
	LongInt( int val ) :_LongInt( val )
	{}
	LongInt( const LFA::string& s, char delimiter = ' ' ) :_LongInt( s, delimiter )
	{}

	LFA::string ToString( char delimiter = 0 ) const
	{
		LFA::string ret;
		ret.reserve( (size_t)n * 10 );
		if( !sign && !isZero() )
			ret += '-';
		char buf[15];
		for( int i = n - 1; i >= 0; --i )
		{
			sprintf_s( buf, ( i == n - 1 ) ? "%u" : "%08u", m_val[i] );
			ret += buf;
			if( delimiter && i != 0 )
				ret += delimiter;
		}
		return ret;
	}

	LongInt operator+( const LongInt& other )const
	{
		LongInt ret;
		SignedPlus( *this, other, sign, other.sign, ret );
		return ret;
	}
	LongInt operator-( const LongInt& other )const
	{
		LongInt ret;
		SignedPlus( *this, other, sign, !other.sign, ret );
		return ret;
	}
private:

};

//RADIX = 1 << 30
class LongBinary :private _LongInt<1 << 30>
{
public:
	~LongBinary()
	{}
	LongBinary() :_LongInt()
	{}
};
}