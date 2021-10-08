#pragma once
#include "CommonDef.h"
#include "VecUtil.h"
#include "Mathematics.h"

namespace Util
{
//Internal
template<unsigned int _RADIX>
class ULongInt
{
public:
	static constexpr unsigned int RADIX = _RADIX;
protected:
	typedef unsigned long long ULL;

	bool sign = true;//postive
	int n = 1;//有效长度
	LFA::vector<unsigned int> m_val;

public:
	ULongInt()
	{
		m_val.resize( 1, 0 );
	}
	ULongInt( int val )
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
	ULongInt( const LFA::string& s, char delimiter = ' ' )
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
	~ULongInt()
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
	bool isEven()const
	{
		assert( n > 0 );
		return ( m_val[0] & 1 ) == 0;
	}
	bool isOdd()const
	{
		return !isEven();
	}
	template<typename Engine = Util::RNG>
	static ULongInt Rand( size_t bit, Engine& rng )
	{
		std::uniform_int_distribution<int>  r( 0, RADIX - 1 );
		ULongInt ret;
		ret.m_val.resize( bit, 0 );
		for( auto& e : ret.m_val )
			e = r( rng );
		ret.n = ret.count_n( (int)bit );
		return ret;
	}
	/*
	bool MillerRabinPrimeTest( size_t numOftest, RNG& rng )
	{
		if( this->operator<( _LongInt( 2 ) ) )
			return false;
		if( this->operator==( _LongInt( 2 ) ) )
			return true;
		if( this->isEven() )
			return false;
		auto randombase = [&] ( _LongInt& val )->void
		{
			val = Rand( this->n, rng );
			val.m_val[this->n - 1] %= m_val[this->n - 1];
			val.n = val.count_n( this->n - 1 );
		};

		_LongInt s = *this - _LongInt( 1 );
		while( s.isEven() )
			s /= 2;
		_LongInt base, temp, mod;
		while( numOftest-- > 0 )
		{
			randombase( base );
			temp = s;
			//UnsignedModPow(base,)
			mod = modulo( base, temp, p );
			while( temp != p - 1 && mod != 1 && mod != p - 1 )
			{
				mod = mulmod( mod, mod, p );
				temp <<= 1;
			}
			if( mod != p - 1 && ( temp & 1 ) == 0 )
				return false;
		}
		return true;
	}*/

protected:
	void swap( ULongInt& other )
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
	int count_n( int from )const
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
	//ignore sign

	bool operator<( const ULongInt& other )const
	{
		if( n != other.n )
			return n < other.n;
		else
		{
			return LT( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
		}
	}
	bool operator>( const ULongInt& other )const
	{
		if( n != other.n )
			return n > other.n;
		else
		{
			return GT( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
		}
	}
	bool operator<=( const ULongInt& other )const
	{
		return !( *this > other );
	}
	bool operator>=( const ULongInt& other )const
	{
		return !( *this < other );
	}
	bool operator==( const ULongInt& other )const
	{
		return n == other.n && EQ( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
	}
	bool operator!=( const ULongInt& other )const
	{
		return !( ULongInt::operator== ( other ) );
	}
	// *= RADIX^bit
	ULongInt& operator<<=( int bit )
	{
		if( bit == 1 )
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

	static void UnsignedPlus( const ULongInt& a, const ULongInt& b, ULongInt& c )
	{
		const ULongInt* n_long = ( a.n > b.n ) ? &a : &b;
		const ULongInt* n_short = ( a.n > b.n ) ? &b : &a;
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

	static void UnsignedSub( const ULongInt& a, const ULongInt& b, ULongInt& c )
	{
		assert( a.n >= b.n );
		assert( a >= b );
		const ULongInt* n_long = &a;
		const ULongInt* n_short = &b;
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

	static void SignedPlus( const ULongInt& a, const ULongInt& b, bool sign_a, bool sign_b, ULongInt& c )
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

	static void UnsignedMultiply( const ULongInt& a, const unsigned int b, ULongInt& c )
	{
		if( b == 0 || a.isZero() )
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
	static void UnsignedMultiply( const ULongInt& a, const ULongInt& b, ULongInt& c )
	{
		const ULongInt* n_long = ( a.n > b.n ) ? &a : &b;
		const ULongInt* n_short = ( a.n > b.n ) ? &b : &a;
		c = ULongInt( 0 );
		thread_local ULongInt tmp;
		tmp.m_val.reserve( a.n + b.n );
		thread_local ULongInt sum;
		sum.m_val.reserve( a.n + b.n );
		int ct = 0;
		for( auto i : n_short->m_val )
		{
			tmp = ULongInt();
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

	static unsigned int UnsignedDivideShort( const ULongInt& a, const ULongInt& b )
	{
		assert( a.n == b.n || a.n == b.n + 1 );
		if( a.operator<( b ) )
			return 0;
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
		thread_local ULongInt tmp;
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
	static void UnsignedDivide( const ULongInt& a, const unsigned int b, ULongInt& quotient, unsigned int& remain )
	{
		assert( a >= ULongInt( b ) );
		assert( b != 0 );
		assert( b < RADIX );
		if( a.operator<( ULongInt( b ) ) )
		{
			assert( a.n == 1 );
			quotient = 0;
			remain = a.m_val[0];
			return;
		}
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
	static void UnsignedDivide( const ULongInt& a, const ULongInt& b, ULongInt& quotient, ULongInt& remain )
	{
		assert( !b.isZero() );
		if( a.operator<( b ) )
		{
			quotient = 0;
			remain = a;
			return;
		}
		quotient.m_val.assign( a.n - b.n + 1, 0 );
		remain.m_val.reserve( a.n );
		remain.m_val.clear();
		remain.n = 0;
		thread_local ULongInt v_mul, v_sub;
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
	static ULongInt UnsignedPow( const ULongInt& base, size_t exponent )
	{
		if( exponent == 0 )
			return ULongInt( 1 );
		thread_local ULongInt tmp;
		return Math::FastExponentiation<ULongInt>( base, exponent, [] ( const ULongInt& l, const ULongInt& r )->ULongInt
		{
			UnsignedMultiply( l, r, tmp );
			return tmp;
		} );
	}
	static ULongInt UnsignedModPow( const ULongInt& base, size_t exponent, const ULongInt& mod )
	{
		/*_LongInt x( 1 );
		_LongInt y = modulo( base, mod );
		while( exponent > 0 )
		{
			if( ( exponent & 1 ) == 1 )
				x = modulo( mul( x, y ), mod );
			y = modulo( mul( y, y ), mod );
			exponent >>= 1;
		}
		return modulo( x, mod );*/
		if( exponent == 0 )
			return ULongInt( 1 );
		thread_local ULongInt tmp, dummy;
		return Math::FastExponentiation<ULongInt>( base, exponent, mod,
												   [] ( const ULongInt& a, const ULongInt& b )->ULongInt
		{
			UnsignedMultiply( a, b, tmp );
			return tmp;
		},
												   [] ( const ULongInt& a, const ULongInt& b )->ULongInt
		{
			UnsignedDivide( a, b, dummy, tmp );
			return tmp;
		} );
	}
};
}