#pragma once
#include "ULongInt.h"

namespace Util
{
//RADIX = 10^8
class LongInt :public ULongInt<100000000>
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
	LongInt() :ULongInt()
	{}
	LongInt( const ULongInt& val ) :ULongInt( val )
	{}
	LongInt( int val ) :ULongInt( val )
	{}
	LongInt( const LFA::string& s, char delimiter = ' ' ) :ULongInt( s, delimiter )
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
		return sign == other.sign && ULongInt::operator==( other );
	}
	bool operator!=( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		return sign != other.sign || ULongInt::operator!=( other );
	}
	bool operator<( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		if( sign != other.sign )
			return sign < other.sign;
		else if( sign && other.sign )
			return ULongInt::operator<( other );
		else
			return ULongInt::operator>( other );
	}
	bool operator>( const LongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		if( sign != other.sign )
			return sign > other.sign;
		else if( sign && other.sign )
			return ULongInt::operator>( other );
		else
			return ULongInt::operator<( other );
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
};

//RADIX = 1 << 30
class LongBinary :private ULongInt<1 << 30>
{
public:
	~LongBinary()
	{}
	LongBinary() :ULongInt()
	{}
};
}