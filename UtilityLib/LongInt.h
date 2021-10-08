#pragma once
#include "ULongInt.h"

namespace Util
{
//RADIX undefined LongInt
template<unsigned int _RADIX>
class RawLongInt;

//RADIX = 10^8
//class LongInt;

class LongInt :public RawULongInt<100000000>
{
protected:
	bool sign = true;//positive or negative

public:
	class Exception_DivByZero :public std::runtime_error
	{
	public:
		Exception_DivByZero() :std::runtime_error( "" )
		{}
	};

	~LongInt()
	{}
	LongInt() :RawULongInt()
	{}
	LongInt( const RawULongInt& val ) :RawULongInt( val )
	{}
	LongInt( int val ) :RawULongInt( abs(val) )
	{
		sign = val > 0;
	}
	LongInt( const LFA::string& s, char delimiter = ' ' ) :RawULongInt( s, delimiter )
	{
		sign = s.empty() || s[0] != '-';
	}

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

	bool isPositive()const noexcept	{		return sign;	}
	bool operator==( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return true;
		return sign == other.sign && RawULongInt::operator==( other );
	}
	bool operator!=( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		return sign != other.sign || RawULongInt::operator!=( other );
	}
	bool operator<( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		if( sign != other.sign )
			return sign < other.sign;
		else if( sign && other.sign )
			return RawULongInt::operator<( other );
		else
			return RawULongInt::operator>( other );
	}
	bool operator>( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		if( sign != other.sign )
			return sign > other.sign;
		else if( sign && other.sign )
			return RawULongInt::operator>( other );
		else
			return RawULongInt::operator<( other );
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
		LongInt q, r;
		UnsignedDivide( *this, other, q, r );
		r.sign = this->sign;
		return r;
	}
	//FastExponentiation
	LongInt operator^( size_t exponent )const
	{
		LongInt r = UnsignedPow( *this, exponent );
		r.sign = this->sign || ( exponent % 2 == 0 );
		return r;
	}
	//FastExponentiation with mod
	LongInt ModPow( size_t exponent, const LongInt& mod )const
	{
		LongInt r = UnsignedModPow( *this, exponent, mod );
		r.sign = this->sign || ( exponent % 2 == 0 );
		return r;
	}

protected:
	static void SignedPlus( const LongInt& a, const LongInt& b, bool sign_a, bool sign_b, LongInt& c )
	{
		if( sign_a == sign_b )
		{
			UnsignedPlus( a, b, c );
			c.sign = sign_a;
		}
		else
		{
			
			const bool ge = a.RawULongInt::operator>=( b );
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
};

//RADIX = 1 << 30
class LongBinary :private RawULongInt<1 << 30>
{
public:
	~LongBinary()
	{}
	LongBinary() :RawULongInt()
	{}
};
}