#pragma once
#include "CommonDef.h"
#include "VecUtil.h"
#include "Mathematics.h"

namespace Util
{
//RADIX undefined ULongInt
template<unsigned int _RADIX>
class RawULongInt
{
public:
	static constexpr unsigned int RADIX = _RADIX;
private:
	static thread_local RawULongInt Dummy;
protected:
	typedef unsigned long long ULL;

	int n = 1;//有效长度
	LFA::vector<unsigned int> m_val;

public:
	RawULongInt()
	{
		m_val.resize( 1, 0 );
	}
	RawULongInt( unsigned int val )
	{
		assert( (ULL)val < (ULL)RADIX * RADIX );
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
	RawULongInt( const LFA::string& s, char delimiter = ' ' )
	{
		if( s.empty() )
		{
			m_val.resize( 1, 0 );
			return;
		}
		const int first = ( s[0] != '-' ) ? 0 : 1;

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
			assert( v < RADIX );
			m_val.emplace_back( v % RADIX );
			if( pos == -1 )
				break;
			offset = pos - 1;
		}
	}
	~RawULongInt()
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
	decltype( n ) GetBit()const noexcept	{		return n;	}
	bool isEven()const
	{
		assert( n > 0 );
		return ( m_val[0] & 1 ) == 0;
	}
	bool isOdd()const
	{
		return !isEven();
	}

	int Compare( const RawULongInt& other )const
	{
		if( n != other.n )
			return n - other.n;
		else
		{
			return Util::Compare( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
		}
	}
	bool operator<( const RawULongInt& other )const
	{
		if( n != other.n )
			return n < other.n;
		else
		{
			return LT( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
		}
	}
	bool operator>( const RawULongInt& other )const
	{
		if( n != other.n )
			return n > other.n;
		else
		{
			return GT( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
		}
	}
	bool operator<=( const RawULongInt& other )const
	{
		return !( *this > other );
	}
	bool operator>=( const RawULongInt& other )const
	{
		return !( *this < other );
	}
	bool operator==( const RawULongInt& other )const
	{
		return n == other.n && EQ( this->m_val.rbegin() + ( m_val.size() - n ), other.m_val.rbegin() + ( other.m_val.size() - n ), n );
	}
	bool operator!=( const RawULongInt& other )const
	{
		return !( RawULongInt::operator== ( other ) );
	}

	RawULongInt& operator++()
	{
		*this += RawULongInt( 1 );
		return *this;
	}
	RawULongInt& operator--()
	{
		*this -= RawULongInt( 1 );
		return *this;
	}
	RawULongInt operator++( int )
	{
		auto tmp = *this;
		*this += RawULongInt( 1 );
		return tmp;
	}
	RawULongInt operator--( int )
	{
		auto tmp = *this;
		*this -= RawULongInt( 1 );
		return tmp;
	}

	RawULongInt operator+( const RawULongInt& other )const
	{
		RawULongInt ret;
		UnsignedPlus( *this, other, ret );
		return ret;
	}
	RawULongInt operator-( const RawULongInt& other )const
	{
		RawULongInt ret;
		UnsignedSub( *this, other, ret );
		return ret;
	}
	RawULongInt operator*( const RawULongInt& other )const
	{
		RawULongInt ret;
		UnsignedMultiply( *this, other, ret );
		return ret;
	}
	RawULongInt operator/( const RawULongInt& other )const
	{
		RawULongInt ret;
		UnsignedDivide( *this, other, ret, Dummy );
		return ret;
	}
	RawULongInt operator%( const RawULongInt& other )const
	{
		RawULongInt ret;
		UnsignedDivide( *this, other, Dummy, ret );
		return ret;
	}

	RawULongInt& operator+=( const RawULongInt& other )
	{
		*this = operator+( other );
		return *this;
	}
	RawULongInt& operator-=( const RawULongInt& other )
	{
		*this = operator-( other );
		return *this;
	}
	RawULongInt& operator*=( const RawULongInt& other )
	{
		*this = operator*( other );
		return *this;
	}
	RawULongInt& operator/=( const RawULongInt& other )
	{
		*this = operator/( other );
		return *this;
	}
	RawULongInt& operator%=( const RawULongInt& other )
	{
		*this = operator%( other );
		return *this;
	}

	//FastExponentiation
	RawULongInt operator^( size_t exponent )const
	{
		return RawULongInt<RADIX>::UnsignedPow( *this, exponent );
	}
	//FastExponentiation with mod
	RawULongInt PowerMod( RawULongInt exponent, const RawULongInt& mod )const
	{
		return RawULongInt<RADIX>::UnsignedPowerMod( *this, exponent, mod );
	}

	template<typename Engine = Util::RNG>
	static RawULongInt Rand( size_t bit, Engine& rng )
	{
		std::uniform_int_distribution<int>  r( 0, RADIX - 1 );
		RawULongInt ret;
		ret.m_val.resize( bit, 0 );
		for( auto& e : ret.m_val )
			e = r( rng );
		ret.n = ret.count_n( (int)bit );
		return ret;
	}
	
	template<typename Engine = Util::RNG>
	bool MillerRabinPrimeTest( size_t numOftest, Engine& rng )const
	{
		if( this->operator<( RawULongInt( 2 ) ) )
			return false;
		if( this->operator==( RawULongInt( 2 ) ) )
			return true;
		if( this->isEven() )
			return false;
		
		auto randombase = [&] ( RawULongInt& val )->void
		{
			assert( this->GetHigh( 1 ) > 0 );
			val = Rand( this->n, rng );
			std::uniform_int_distribution<int>  r( 0, this->GetHigh( 1 ) - 1 );
			val.m_val[this->n - 1] = r( rng );
			val.n = val.count_n( this->n );
			if( val <= RawULongInt( 1 ) )
				val = 2;
			assert( val < *this );
		};

		RawULongInt s = *this - RawULongInt( 1 );
		int r = 0;
		while( s.isEven() && !s.isZero() )
		{
			s /= 2;
			++r;
		}
		RawULongInt base, mod;
		const RawULongInt p_1 = *this - 1;
		while( numOftest-- > 0 )
		{
			randombase( base );
			mod = base.PowerMod( s, *this );
			if( mod == RawULongInt( 1 ) || mod == p_1 )
				continue;
			bool flag = false;
			for( int i = 0; i < r - 1; i++ )
			{
				mod = ( mod * mod ) % *this;
				if( mod == p_1 )
				{
					flag = true;
					break;
				}
			}
			if( !flag )
				return false;
		}
		return true;
	}

protected:
	void swap( RawULongInt& other )
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
	
	// *= RADIX^bit
	RawULongInt& operator<<=( int bit )
	{
		if( bit == 1 )
		{
			m_val.insert( m_val.begin(), 0 );
			n += bit;
		}
		else if( bit > 1 )
		{
			thread_local decltype(m_val) tmp;
			tmp.assign( bit, 0 );
			m_val.insert( m_val.begin(), tmp.begin(), tmp.end() );
			n += bit;
		}
		return *this;
	}

	static void UnsignedPlus( const RawULongInt& a, const RawULongInt& b, RawULongInt& c )
	{
		const RawULongInt* n_long = ( a.n > b.n ) ? &a : &b;
		const RawULongInt* n_short = ( a.n > b.n ) ? &b : &a;
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

	static void UnsignedSub( const RawULongInt& a, const RawULongInt& b, RawULongInt& c )
	{
		assert( a.n >= b.n );
		assert( a >= b );
		const RawULongInt* n_long = &a;
		const RawULongInt* n_short = &b;
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

	static void UnsignedMultiply( const RawULongInt& a, const unsigned int b, RawULongInt& c )
	{
		if( b == 0 || a.isZero() )
		{
			c = 0;
			return;
		}
		if( b == 1 )
		{
			c = a;
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
	static void UnsignedMultiply( const RawULongInt& a, const RawULongInt& b, RawULongInt& c )
	{
		const RawULongInt* n_long = ( a.n > b.n ) ? &a : &b;
		const RawULongInt* n_short = ( a.n > b.n ) ? &b : &a;
		c = 0;
		thread_local RawULongInt tmp;
		tmp.m_val.reserve( a.n + b.n );
		thread_local RawULongInt sum;
		sum.m_val.reserve( a.n + b.n );
		c.m_val.reserve( a.n + b.n );
		int ct = 0;
		for( int i = 0; i < n_short->n; i++ )
		{
			UnsignedMultiply( *n_long, n_short->m_val[i], tmp );
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

	static unsigned int UnsignedDivideShort( const RawULongInt& a, const RawULongInt& b )
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
			r = (unsigned int)std::min( (ULL)r, x / b.GetHigh() );
			l = (unsigned int)( x / ( b.GetHigh() + 1 ) );
		}
		assert( l <= r );
		thread_local RawULongInt tmp;
		while( l < r )
		{
			auto mid = ( l + r + 1 ) >> 1;
			UnsignedMultiply( b, mid, tmp );
			const int cmp = tmp.Compare( a );
			if( cmp > 0 )
				r = mid - 1;
			else if( cmp < 0 )
				l = mid;
			else
				return mid;
		}
		return l;
	}
	static void UnsignedDivide( const RawULongInt& a, const unsigned int b, RawULongInt& quotient, unsigned int& remain )
	{
		assert( a >= RawULongInt( b ) );
		assert( b != 0 );
		assert( b < RADIX );
		if( a.operator<( RawULongInt( b ) ) )
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
	static void UnsignedDivide( const RawULongInt& a, const RawULongInt& b, RawULongInt& quotient, RawULongInt& remain )
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
		thread_local RawULongInt v_mul, v_sub;
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
		assert( quotient * b + remain == a );
	}
	static RawULongInt UnsignedPow( const RawULongInt& base, size_t exponent )
	{
		if( exponent == 0 )
			return RawULongInt( 1 );
		if( exponent == 1 )
			return base;
		RawULongInt x( 1 );
		thread_local RawULongInt y;
		x.m_val.reserve( base.n * exponent + 1 );
		y.m_val.reserve( base.n * exponent + 1 );
		y = base;
		while( exponent )
		{
			if( exponent & 1 )
				x *= y;
			exponent >>= 1;
			if( exponent )
				y *= y;
		}
		return x;
	}
	static RawULongInt UnsignedPowerMod( const RawULongInt& base, RawULongInt exponent, const RawULongInt& mod )
	{
		if( exponent == 0 )
			return RawULongInt( 1 );
		if( exponent == 1 )
			return base % mod;
		RawULongInt x( 1 );
		thread_local RawULongInt y;
		x.m_val.reserve( base.n * 2 + 1 );
		y.m_val.reserve( base.n * 2 + 1 );
		y = base % mod;
		while( !exponent.isZero() )
		{
			if( exponent.isOdd() )
			{
				x *= y;
				x %= mod;
			}
			y *= y;
			y %= mod;
			exponent /= 2;
		}
		return x;
	}
};

template<unsigned int _RADIX>
thread_local RawULongInt<_RADIX> RawULongInt<_RADIX>::Dummy( 0 );
}