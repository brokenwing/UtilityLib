#include "pch.h"
#include "Matrix.h"

using namespace Util;

TEST( Matrix, Construction_empty_size )
{
	Matrix<int> mat1;
	EXPECT_TRUE( mat1.empty() );
	EXPECT_EQ( mat1.size(), std::make_pair( size_t( 0 ), size_t( 0 ) ) );

	Matrix<int> mat2( 3, 4 );
	EXPECT_FALSE( mat2.empty() );
	EXPECT_EQ( mat2.size(), std::make_pair( size_t( 3 ), size_t( 4 ) ) );

	Matrix<int> mat3( 3, 3, 1 );
	EXPECT_FALSE( mat3.empty() );
	EXPECT_EQ( mat3.size(), std::make_pair( size_t( 3 ), size_t( 3 ) ) );
}

TEST( Matrix, Construction_initializer_list )
{
	Matrix<int> mat( { {1,2},{3,4} } );
	EXPECT_EQ( mat.size(), std::make_pair( size_t( 2 ), size_t( 2 ) ) );
	EXPECT_EQ( mat[0][0], 1 );
	EXPECT_EQ( mat[1][1], 4 );
	EXPECT_EQ( mat[0][1], 2 );
	EXPECT_EQ( mat[1][0], 3 );
}

TEST( Matrix, Construction_initializer_list_lack )
{
	Matrix<int> mat( { {1,2,3},{1,2},{1} } );
	EXPECT_EQ( mat.size(), std::make_pair( size_t( 3 ), size_t( 3 ) ) );
	EXPECT_EQ( mat[0][0], 1 );
	EXPECT_EQ( mat[0][1], 2 );
	EXPECT_EQ( mat[0][2], 3 );
	EXPECT_EQ( mat[1][0], 1 );
	EXPECT_EQ( mat[1][1], 2 );
	EXPECT_EQ( mat[2][0], 1 );
}

TEST( Matrix, Construction_at )
{
	Matrix<int> mat( 3, 4, 1 );
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 4; j++ )
			EXPECT_EQ( mat.at( i, j ), 1 );
	mat.at( 0, 0 ) *= -1;
	EXPECT_EQ( mat.at( 0, 0 ), -1 );
}

TEST( Matrix, Construction_bracket )
{
	Matrix<int> mat( 3, 4, 1 );
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 4; j++ )
			EXPECT_EQ( mat[i][j], 1 );
	mat[0][0] *= -1;
	EXPECT_EQ( mat[0][0], -1 );
}

TEST( Matrix, swap )
{
	Matrix<int> mat1( 3, 3, 3 );
	Matrix<int> mat2( 2, 2, 2 );
	mat1.swap( mat2 );
	ASSERT_EQ( mat1.size(), std::make_pair( size_t( 2 ), size_t( 2 ) ) );
	ASSERT_EQ( mat2.size(), std::make_pair( size_t( 3 ), size_t( 3 ) ) );
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 3; j++ )
			EXPECT_EQ( mat2[i][j], 3 );
	for( int i = 0; i < 2; i++ )
		for( int j = 0; j < 2; j++ )
			EXPECT_EQ( mat1[i][j], 2 );
}

TEST( Matrix, fill )
{
	Matrix<int> mat( 3, 4, 1 );
	mat.fill( -1 );
	for( int i = 0; i < 3; i++ )
		for( int j = 0; j < 4; j++ )
			EXPECT_EQ( mat[i][j], -1 );
}

TEST( Matrix, clear )
{
	Matrix<int> mat( 3, 4, 1 );
	mat.clear();
	EXPECT_TRUE( mat.empty() );
}

TEST( Matrix, resize )
{
	Matrix<int> mat;
	mat.resize( 2, 3 );
	EXPECT_EQ( mat.size(), std::make_pair( size_t( 2 ), size_t( 3 ) ) );
	mat.resize( 3, 2 );
	EXPECT_EQ( mat.size(), std::make_pair( size_t( 3 ), size_t( 2 ) ) );
	mat.resize( 1, 1, 1 );
	EXPECT_EQ( mat[0][0], 1 );
}

TEST( Matrix, equal )
{
	Matrix<int> mat1( 3, 4, 1 );
	Matrix<int> mat2( 3, 4, 1 );
	EXPECT_TRUE( mat1 == mat2 );
	mat2[0][0] = 0;
	EXPECT_FALSE( mat1 == mat2 );
	EXPECT_FALSE( mat1 == Matrix<int>() );
}

TEST( Matrix, transpose_nn )
{
	Matrix<int> mat( 2, 2 );
	mat[0][0] = 1;
	mat[0][1] = 2;
	mat[1][0] = 3;
	mat[1][1] = 4;
	mat.Transpose();
	EXPECT_EQ( mat[0][0], 1 );
	EXPECT_EQ( mat[1][1], 4 );
	EXPECT_EQ( mat[0][1], 3 );
	EXPECT_EQ( mat[1][0], 2 );
}

TEST( Matrix, transpose_nm )
{
	//1 2 3
	//4 5 6
	Matrix<int> mat( 2, 3 );
	mat[0][0] = 1;
	mat[0][1] = 2;
	mat[0][2] = 3;
	mat[1][0] = 4;
	mat[1][1] = 5;
	mat[1][2] = 6;
	mat.Transpose();
	ASSERT_EQ( mat.size().first, 3 );
	ASSERT_EQ( mat.size().second, 2 );
	//1 4
	//2 5
	//3 6
	EXPECT_EQ( mat[0][0], 1 );
	EXPECT_EQ( mat[1][0], 2 );
	EXPECT_EQ( mat[2][0], 3 );
	EXPECT_EQ( mat[0][1], 4 );
	EXPECT_EQ( mat[1][1], 5 );
	EXPECT_EQ( mat[2][1], 6 );
}

TEST( Matrix, MatrixPlus )
{
	Matrix<int> a(2,2,1);
	Matrix<int> b( { {1,2},{3,4} } );
	Matrix<int> c;
	c = a + b;
	EXPECT_EQ( c[0][0], 2 );
	EXPECT_EQ( c[0][1], 3 );
	EXPECT_EQ( c[1][0], 4 );
	EXPECT_EQ( c[1][1], 5 );
}

TEST( Matrix, MatrixMinus )
{
	Matrix<int> a(2,2,1);
	Matrix<int> b( { {1,2},{3,4} } );
	Matrix<int> c;
	c = b - a;
	EXPECT_EQ( c[0][0], 0 );
	EXPECT_EQ( c[0][1], 1 );
	EXPECT_EQ( c[1][0], 2 );
	EXPECT_EQ( c[1][1], 3 );
}

TEST( Matrix, MatrixMul )
{
	Matrix<int> a(2,2,3);
	Matrix<int> b( { {1,2},{3,4} } );
	Matrix<int> c;
	c = a * b;
	EXPECT_EQ( c[0][0], 3 );
	EXPECT_EQ( c[0][1], 6 );
	EXPECT_EQ( c[1][0], 9 );
	EXPECT_EQ( c[1][1], 12 );
}

TEST( Matrix, MatrixDiv )
{
	Matrix<int> a(2,2,2);
	Matrix<int> b( { {2,4},{6,8} } );
	Matrix<int> c;
	c = b / a;
	EXPECT_EQ( c[0][0], 1 );
	EXPECT_EQ( c[0][1], 2 );
	EXPECT_EQ( c[1][0], 3 );
	EXPECT_EQ( c[1][1], 4 );
}

TEST( Matrix, PlusNumber )
{
	Matrix<int> a( { {1,2},{3,4} } );
	Matrix<int> c;
	c = a + 1;
	EXPECT_EQ( c[0][0], 2 );
	EXPECT_EQ( c[0][1], 3 );
	EXPECT_EQ( c[1][0], 4 );
	EXPECT_EQ( c[1][1], 5 );
}

TEST( Matrix, MinusNumber )
{
	Matrix<int> a( { {1,2},{3,4} } );
	Matrix<int> c;
	c = a - 1;
	EXPECT_EQ( c[0][0], 0 );
	EXPECT_EQ( c[0][1], 1 );
	EXPECT_EQ( c[1][0], 2 );
	EXPECT_EQ( c[1][1], 3 );
}

TEST( Matrix, MulNumber )
{
	Matrix<int> a( { {1,2},{3,4} } );
	Matrix<int> c;
	c = a * 2;
	EXPECT_EQ( c[0][0], 2 );
	EXPECT_EQ( c[0][1], 4 );
	EXPECT_EQ( c[1][0], 6 );
	EXPECT_EQ( c[1][1], 8 );
}

TEST( Matrix, DivNumber )
{
	Matrix<int> a( { {2,4},{6,8} } );
	Matrix<int> c;
	c = a / 2;
	EXPECT_EQ( c[0][0], 1 );
	EXPECT_EQ( c[0][1], 2 );
	EXPECT_EQ( c[1][0], 3 );
	EXPECT_EQ( c[1][1], 4 );
}

TEST( Matrix, MatMul_nn )
{
	Matrix<int> a( { {1,2},{3,4} } );
	Matrix<int> b( { {0,1},{1,0} } );
	Matrix<int> c;
	c = a.MatMul( b );
	EXPECT_EQ( c[0][0], 2 );
	EXPECT_EQ( c[0][1], 1 );
	EXPECT_EQ( c[1][0], 4 );
	EXPECT_EQ( c[1][1], 3 );
}

TEST( Matrix, MatMul_nm )
{
	Matrix<int> a( { {1,2,3},{4,5,6} } );
	Matrix<int> b( { {1,0},{1,1},{0,1} } );
	Matrix<int> c;
	c = a.MatMul( b );
	EXPECT_EQ( c[0][0], 3 );
	EXPECT_EQ( c[0][1], 5 );
	EXPECT_EQ( c[1][0], 9 );
	EXPECT_EQ( c[1][1], 11 );
}

TEST( Matrix, Diagonal )
{
	Matrix<int> c = Matrix<int>::Diagonal( 2 );
	EXPECT_EQ( c[0][0], 1 );
	EXPECT_EQ( c[0][1], 0 );
	EXPECT_EQ( c[1][0], 0 );
	EXPECT_EQ( c[1][1], 1 );
}