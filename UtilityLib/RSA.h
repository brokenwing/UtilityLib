#pragma once
#include "Mathematics.h"
#include "RawLongInt.h"

namespace Util
{
template<typename _LongInt = RawLongInt<1 << 30>>
class RSA
{
public:
	using ValueType = _LongInt;
	struct RSA_Key
	{
		_LongInt N, x;
		_LongInt Encrypt( const _LongInt& data )const
		{
			return data.PowerMod( x, N );
		}
	};
private:
	int N_PRIME_TEST = 10;
	_LongInt N, e, d;

public:
	RSA()
	{}
	~RSA()
	{}

	RSA_Key GetPublicKey()const noexcept
	{
		RSA_Key ret;
		ret.N = N;
		ret.x = e;
		return ret;
	}
	_LongInt Decrypt( const _LongInt& data )const noexcept
	{
		return data.PowerMod( d, N );
	}
	void Generate( size_t min_binarybit = 512 )
	{
		const size_t n = 1 + min_binarybit / 30;
		std::random_device rd;
		RNG rng( rd() );
		
		_LongInt p, q;
		p = GeneratePrime( n, rd, rng );
		do
		{
			q = GeneratePrime( n, rd, rng );
		} while( p == q );
		
		N = p * q;
		_LongInt r = ( p - 1 ) * ( q - 1 );
		do
		{
			e = _LongInt::Rand( r.GetBit() / 2 + 1, rd );
		} while( e >= r || Math::GCD<_LongInt>( e, r ) != 1 );
		d = Math::Inverse<_LongInt>( e, r );
		assert( ( e * d ) % r == 1 );
	}

private:
	template <typename RD, typename RNG>
	_LongInt GeneratePrime( const size_t n, RD& rd, RNG& rng )const
	{
		_LongInt p;
		do
		{
			p = _LongInt::Rand( n, rd );
		} while( p.GetBit() < n );
		while( !p.MillerRabinPrimeTest( N_PRIME_TEST, rng ) )
		{
			++p;
		}
		return p;
	}
};
}