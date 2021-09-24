#pragma once
#include "CommonDef.h"
#include "VecUtil.h"
#include "Mathematics.h"

namespace Util
{
//Internal
template<unsigned int _RADIX>
class _LongInt
{
public:
	static constexpr unsigned int RADIX = _RADIX;
protected:
	typedef unsigned long long ULL;

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
		assert( (ULL)abs( val ) < (ULL)RADIX * RADIX );
		sign = val >= 0;
		val = abs( val );
		if( val < RADIX )
		{
			m_val.resize( 1, val );
			n = 1;
			return;
		}
		n = 0;
		while( val )
		{
			++n;
			m_val.emplace_back( val % RADIX );
			val /= RADIX;
		}
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
	unsigned int GetHigh( int nth = 1 )const
	{
		assert( n >= nth );
		return m_val[n - nth];
	}
	unsigned int GetLow( int nth = 1 )const
	{
		return m_val[nth - 1];
	}
	static _LongInt Rand( size_t bit, RNG& rng )
	{
		std::uniform_int_distribution<int>  r( 0, RADIX - 1 );
		_LongInt ret;
		ret.m_val.resize( bit, 0 );
		for( auto& e : ret.m_val )
			e = r( rng );
		ret.n = ret.count_n( (int)bit );
		return ret;
	}

protected:
	void swap( _LongInt& other )
	{
		m_val.swap( other.m_val );
		std::swap( n, other.n );
	}
	bool check()const
	{
		if( n < 1 )
			return false;
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
	// *= RADIX^bit
	_LongInt& operator<<=( int bit )
	{
		if( bit ==1 )
		{
			m_val.insert( m_val.begin(), 0 );
			n += bit;
		}
		else if( bit > 1 )
		{
			thread_local LFA::vector<int> tmp;
			tmp.assign( bit, 0 );
			m_val.insert( m_val.begin(), tmp.begin(), tmp.end() );
			n += bit;
		}
		return *this;
	}

	static void UnsignedPlus( const _LongInt & a, const _LongInt & b, _LongInt & c )
	{
		const _LongInt* n_long = ( a.n > b.n ) ? &a : &b;
		const _LongInt* n_short = ( a.n > b.n ) ? &b : &a;
		c.m_val.assign( (size_t)n_long->n + 1, 0 );
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
	
	static void UnsignedSub( const _LongInt& a, const _LongInt& b, _LongInt& c )
	{
		assert( a.n >= b.n );
		assert( a >= b );
		const _LongInt* n_long = &a;
		const _LongInt* n_short = &b;
		c.m_val.assign( n_long->n, 0 );
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
				UnsignedSub( a, b, c );
			else
				UnsignedSub( b, a, c );
			//a b   c
			//+ - > +
			//+ - < -
			//- + > -
			//- + < +
			c.sign = ge ^ sign_b;
		}
	}
	
	static void UnsignedMultiply( const _LongInt& a, const unsigned int b, _LongInt& c )
	{
		if( b == 0 || a == _LongInt( 0 ) )
		{
			c.n = 1;
			c.m_val.assign( 1, 0 );
			return;
		}
		if( b == 1 )
		{
			c.n = a.n;
			c.m_val = a.m_val;
			return;
		}
		assert( b >= 0 && b < RADIX );
		c.m_val.assign( (size_t)a.n + 1, 0 );
		c.n = a.n;
		ULL offset = 0;
		auto it = c.m_val.begin();
		auto it_a = a.m_val.begin();
		for( int i = 0; i < a.n; i++, ++it, ++it_a )
		{
			ULL tmp = offset + *it_a * (ULL)b;
			*it = tmp % RADIX;
			offset = tmp / RADIX;
		}
		if( offset != 0 )
		{
			assert( offset < RADIX );
			*it = (unsigned int)offset;
			++c.n;
		}
		assert( c.check() );
	}
	static void UnsignedMultiply( const _LongInt& a, const _LongInt &b, _LongInt& c )
	{
		const _LongInt* n_long = ( a.n > b.n ) ? &a : &b;
		const _LongInt* n_short = ( a.n > b.n ) ? &b : &a;
		c = _LongInt( 0 );
		thread_local _LongInt tmp;
		tmp.m_val.reserve( a.n + b.n );
		thread_local _LongInt sum;
		sum.m_val.reserve( a.n + b.n );
		int ct = 0;
		for( auto i : n_short->m_val )
		{
			tmp = _LongInt();
			UnsignedMultiply( *n_long, i, tmp );
			if( !tmp.isZero() )
			{
				tmp <<= ct;// *= RADIX
				UnsignedPlus( tmp, c, sum );
				c.swap( sum );
			}
			ct++;
		}
		assert( c.check() );
	}

	static unsigned int UnsignedDivideShort( const _LongInt& a, const _LongInt& b )
	{
		assert( a >= b );
		assert( a.n == b.n || a.n == b.n + 1 );
		unsigned int l = 0;
		unsigned int r = RADIX - 1;
		if( a.n == b.n )
		{
			r = a.GetHigh() / b.GetHigh();
			l = a.GetHigh() / ( b.GetHigh() + 1 );
		}
		else if( a.n == b.n + 1 )
		{
			ULL x = (ULL)a.GetHigh() * RADIX + a.GetHigh( 2 );
			assert( x / b.GetHigh() < RADIX );
			r = (unsigned int)( x / b.GetHigh() );
			l = (unsigned int)( x / ( b.GetHigh() + 1 ) );
		}
		assert( l <= r );
		thread_local _LongInt tmp;
		while( l < r )
		{
			auto mid = ( l + r + 1 ) >> 1;
			UnsignedMultiply( b, mid, tmp );
			if( tmp > a )
				r = mid - 1;
			else if( tmp < a )
				l = mid;
			else
				return mid;
		}
		return l;
	}
	static void UnsignedDivide( const _LongInt& a, const unsigned int b, _LongInt& quotient, unsigned int& remain )
	{
		assert( a >= _LongInt( b ) );
		assert( b != 0 );
		assert( b < RADIX );
		quotient = a;
		remain = 0;
		for( int i = a.n - 1; i >= 0; i-- )
		{
			ULL tmp = (ULL)remain * RADIX + a.m_val[i];
			assert( tmp / b < RADIX );
			quotient.m_val[i] = (unsigned int)( tmp / b );
			remain = (unsigned int)( tmp % b );
		}
		quotient.n = quotient.count_n( a.n );
	}
	static void UnsignedDivide( const _LongInt& a, const _LongInt& b, _LongInt& quotient, _LongInt& remain )
	{
		assert( a >= b );
		assert( !b.isZero() );
		quotient.m_val.assign( a.n - b.n + 1, 0 );
		remain.m_val.reserve( a.n );
		remain.m_val.clear();
		remain.n = 0;
		thread_local _LongInt v_mul, v_sub;
		for( int pos = a.n - 1; pos >= 0; --pos )
		{
			remain.m_val.insert( remain.m_val.begin(), a.m_val[pos] );
			++remain.n;
			if( remain < b )
				continue;
			auto q = UnsignedDivideShort( remain, b );
			quotient.m_val[pos] = q;
			UnsignedMultiply( b, q, v_mul );
			UnsignedSub( remain, v_mul, v_sub );
			remain.swap( v_sub );
			if( remain.isZero() )
				remain.n = 0;
		}
		if( remain.n == 0 )
			remain.n = 1;
		quotient.n = quotient.count_n( a.n - b.n + 1 );
	}
};
;
//RADIX = 10^8
class LongInt :public _LongInt<100000000>
{
public:
	class Exception_DivByZero :public std::runtime_error
	{
	public:
		Exception_DivByZero() :std::runtime_error( "" )
		{}
	};

	~LongInt()
	{}
	LongInt() :_LongInt()
	{}
	LongInt( const _LongInt& val ) :_LongInt( val )
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

	bool operator==( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return true;
		return sign == other.sign && _LongInt::operator==( other );
	}
	bool operator!=( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		return sign != other.sign || _LongInt::operator!=( other );
	}
	bool operator<( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		if( sign != other.sign )
			return sign < other.sign;
		else if( sign && other.sign )
			return _LongInt::operator<( other );
		else
			return _LongInt::operator>( other );
	}
	bool operator>( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		if( sign != other.sign )
			return sign > other.sign;
		else if( sign && other.sign )
			return _LongInt::operator>( other );
		else
			return _LongInt::operator<( other );
	}
	bool operator<=( const LongInt& other )const
	{
		return !( *this > other );
	}
	bool operator>=( const LongInt& other )const
	{
		return !( *this < other );
	}

	LongInt& operator++()
	{
		*this += LongInt( 1 );
		return *this;
	}
	LongInt& operator--()
	{
		*this -= LongInt( 1 );
		return *this;
	}
	LongInt operator++(int)
	{
		auto tmp = *this;
		*this += LongInt( 1 );
		return tmp;
	}
	LongInt operator--(int)
	{
		auto tmp = *this;
		*this -= LongInt( 1 );
		return tmp;
	}
	LongInt& operator+=( const LongInt& other )
	{
		*this = operator+( other );
		return *this;
	}
	LongInt& operator-=( const LongInt& other )
	{
		*this = operator-( other );
		return *this;
	}
	LongInt& operator*=( const LongInt& other )
	{
		*this = operator*( other );
		return *this;
	}
	LongInt& operator/=( const LongInt& other )
	{
		*this = operator/( other );
		return *this;
	}
	LongInt& operator%=( const LongInt& other )
	{
		*this = operator%( other );
		return *this;
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
	LongInt operator*( int other )const
	{
		if( other < RADIX )
		{
			LongInt ret;
			UnsignedMultiply( *this, abs( other ), ret );
			ret.sign = this->sign == ( other >= 0 );
			return ret;
		}
		else
			return *this * LongInt( other );
	}
	LongInt operator*( const LongInt &other )const
	{
		LongInt ret;
		UnsignedMultiply( *this, other, ret );
		ret.sign = this->sign == other.sign;
		return ret;
	}
	LongInt operator/( const int other )const
	{
		if( this->isZero() )
			return LongInt( 0 );
		if( other == 0 )
			throw Exception_DivByZero();
		if( _LongInt::operator<( other ) )
			return LongInt( 0 );
		if( abs( other ) < RADIX )
		{
			unsigned int r;
			LongInt q;
			UnsignedDivide( *this, abs( other ), q, r );
			q.sign = this->sign == ( other > 0 );
			return q;
		}
		else
			return *this / LongInt( other );
	}
	LongInt operator%( const int other )const
	{
		if( this->isZero() )
			return LongInt( 0 );
		if( other == 0 )
			throw Exception_DivByZero();
		if( _LongInt::operator<( other ) )
			return LongInt( 0 );
		if( abs( other ) < RADIX )
		{
			unsigned int r;
			LongInt q;
			UnsignedDivide( *this, abs( other ), q, r );
			
			return sign ? r : -(int)r;
		}
		else
			return *this % LongInt( other );
	}
	LongInt operator/( const LongInt& other )const
	{
		if( this->isZero() )
			return LongInt( 0 );
		if( other.isZero() )
			throw Exception_DivByZero();
		if( _LongInt::operator<( other ) )
			return LongInt( 0 );
		LongInt q, r;
		UnsignedDivide( *this, other, q, r );
		q.sign = this->sign == other.sign;
		return q;
	}
	LongInt operator%( const LongInt& other )const
	{
		if( this->isZero() )
			return LongInt( 0 );
		if( other.isZero() )
			throw Exception_DivByZero();
		if( _LongInt::operator<( other ) )
			return *this;
		LongInt q, r;
		UnsignedDivide( *this, other, q, r );
		r.sign = this->sign;
		return r;
	}
	//FastExponentiation
	LongInt operator^( unsigned int exponent )const
	{
		if( exponent == 0 )
			return LongInt( 1 );
		return Math::FastExponentiation( *this, exponent, [] ( const LongInt& l, const LongInt& r )
		{
			return l * r;
		} );
	}
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