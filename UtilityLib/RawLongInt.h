#pragma once
#include "ULongInt.h"

namespace Util
{
//RADIX undefined LongInt
template<unsigned int _RADIX>
class RawLongInt :public RawULongInt<_RADIX>
{
public:
	static constexpr unsigned int RADIX = _RADIX;
private:
	static thread_local RawULongInt<_RADIX> Dummy;
protected:
	bool sign = true;//positive or negative

public:
	class Exception_DivByZero :public std::runtime_error
	{
	public:
		Exception_DivByZero() :std::runtime_error( "" )
		{}
	};

	~RawLongInt()
	{}
	RawLongInt() :RawULongInt<RADIX>()
	{}
	RawLongInt( const RawULongInt<_RADIX>& val ) :RawULongInt<RADIX>( val )
	{}
	RawLongInt( int val ) :RawULongInt<RADIX>( abs( val ) )
	{
		sign = val > 0;
	}
	RawLongInt( const LFA::string& s, char delimiter = ' ' ) :RawULongInt<RADIX>( s, delimiter )
	{
		sign = s.empty() || s[0] != '-';
	}

	bool isPositive()const noexcept
	{
		return sign;
	}
	bool operator==( const RawLongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return true;
		return sign == other.sign && RawULongInt<RADIX>::operator==( other );
	}
	bool operator!=( const RawLongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		return sign != other.sign || RawULongInt<RADIX>::operator!=( other );
	}
	bool operator<( const RawLongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		if( sign != other.sign )
			return sign < other.sign;
		else if( sign && other.sign )
			return RawULongInt<RADIX>::operator<( other );
		else
			return RawULongInt<RADIX>::operator>( other );
	}
	bool operator>( const RawLongInt& other )const
	{
		if( this->isZero() && other.isZero() )
			return false;
		if( sign != other.sign )
			return sign > other.sign;
		else if( sign && other.sign )
			return RawULongInt<RADIX>::operator>( other );
		else
			return RawULongInt<RADIX>::operator<( other );
	}
	bool operator<=( const RawLongInt& other )const
	{
		return !( *this > other );
	}
	bool operator>=( const RawLongInt& other )const
	{
		return !( *this < other );
	}

	RawLongInt& operator++()
	{
		*this += RawLongInt( 1 );
		return *this;
	}
	RawLongInt& operator--()
	{
		*this -= RawLongInt( 1 );
		return *this;
	}
	RawLongInt operator++( int )
	{
		auto tmp = *this;
		*this += RawLongInt( 1 );
		return tmp;
	}
	RawLongInt operator--( int )
	{
		auto tmp = *this;
		*this -= RawLongInt( 1 );
		return tmp;
	}
	RawLongInt& operator+=( const RawLongInt& other )
	{
		*this = operator+( other );
		return *this;
	}
	RawLongInt& operator-=( const RawLongInt& other )
	{
		*this = operator-( other );
		return *this;
	}
	RawLongInt& operator*=( const RawLongInt& other )
	{
		*this = operator*( other );
		return *this;
	}
	RawLongInt& operator/=( const RawLongInt& other )
	{
		*this = operator/( other );
		return *this;
	}
	RawLongInt& operator%=( const RawLongInt& other )
	{
		*this = operator%( other );
		return *this;
	}

	RawLongInt operator+( const RawLongInt& other )const
	{
		return SignedPlus( *this, other, sign, other.sign );;
	}
	RawLongInt operator-( const RawLongInt& other )const
	{
		return SignedPlus( *this, other, sign, !other.sign );
	}
	RawLongInt operator*( int other )const
	{
		if( other < RADIX )
		{
			RawLongInt ret;
			RawULongInt<RADIX>::UnsignedMultiply( *this, abs( other ), ret );
			ret.sign = this->sign == ( other >= 0 );
			return ret;
		}
		else
			return *this * RawLongInt( other );
	}
	RawLongInt operator*( const RawLongInt& other )const
	{
		RawLongInt ret;
		RawULongInt<RADIX>::UnsignedMultiply( *this, other, ret );
		ret.sign = this->sign == other.sign;
		return ret;
	}
	RawLongInt operator/( const int other )const
	{
		if( this->isZero() )
			return RawLongInt( 0 );
		if( other == 0 )
			throw Exception_DivByZero();
		if( abs( other ) < RADIX )
		{
			unsigned int r;
			RawLongInt q;
			RawULongInt<RADIX>::UnsignedDivide( *this, abs( other ), q, r );
			q.sign = this->sign == ( other > 0 );
			return q;
		}
		else
			return *this / RawLongInt( other );
	}
	RawLongInt operator%( const int other )const
	{
		if( this->isZero() )
			return RawLongInt( 0 );
		if( other == 0 )
			throw Exception_DivByZero();
		if( abs( other ) < RADIX )
		{
			unsigned int r;
			RawULongInt<RADIX>::UnsignedDivide( *this, abs( other ), Dummy, r );
			return sign ? r : -(int)r;
		}
		else
			return *this % RawLongInt( other );
	}
	RawLongInt operator/( const RawLongInt& other )const
	{
		if( this->isZero() )
			return RawLongInt( 0 );
		if( other.isZero() )
			throw Exception_DivByZero();
		RawLongInt q;
		RawULongInt<RADIX>::UnsignedDivide( *this, other, q, Dummy );
		q.sign = this->sign == other.sign;
		return q;
	}
	RawLongInt operator%( const RawLongInt& other )const
	{
		if( this->isZero() )
			return RawLongInt( 0 );
		if( other.isZero() )
			throw Exception_DivByZero();
		RawLongInt r;
		RawULongInt<RADIX>::UnsignedDivide( *this, other, Dummy, r );
		r.sign = this->sign;
		return r;
	}
	//FastExponentiation
	RawLongInt operator^( size_t exponent )const
	{
		RawLongInt r = RawULongInt<RADIX>::UnsignedPow( *this, exponent );
		r.sign = this->sign || ( exponent % 2 == 0 );
		return r;
	}
	//FastExponentiation with mod
	RawLongInt PowerMod( RawULongInt<RADIX> exponent, const RawLongInt& mod )const
	{
		RawLongInt r = RawULongInt<RADIX>::UnsignedPowerMod( *this, exponent, mod );
		r.sign = this->sign || ( exponent % 2 == 0 );
		return r;
	}

protected:
	static RawLongInt SignedPlus( const RawLongInt& a, const RawLongInt& b, bool sign_a, bool sign_b )
	{
		RawLongInt c;
		if( sign_a == sign_b )
		{
			RawLongInt<RADIX>::UnsignedPlus( a, b, c );
			c.sign = sign_a;
		}
		else
		{

			const bool ge = a.RawULongInt<RADIX>::operator>=( b );
			if( ge )
				RawLongInt<RADIX>::UnsignedSub( a, b, c );
			else
				RawLongInt<RADIX>::UnsignedSub( b, a, c );
			//a b   c
			//+ - > +
			//+ - < -
			//- + > -
			//- + < +
			c.sign = ge ^ sign_b;
		}
		return c;
	}
};

template<unsigned int _RADIX>
thread_local RawULongInt<_RADIX> RawLongInt<_RADIX>::Dummy( 0 );
}