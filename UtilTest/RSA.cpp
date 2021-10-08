#include "pch.h"
#include "RSA.h"

using namespace Util;

TEST( RSA, basic )
{
	RSA rsa;
	rsa.Generate( 64 );
	auto key = rsa.GetPublicKey();
	auto code = key.Encrypt( 12345 );
	auto r = rsa.Decrypt( code );
	EXPECT_EQ( r, 12345 );
}