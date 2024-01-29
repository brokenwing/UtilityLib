#include "pch.h"
#include "SimplexMethod.h"
#include "CommonDef.h"

#include <random>

using namespace Util::ORtool;

TEST( SimplexMethod, compile )
{
	SimplexMethod sm;
	NonStandardFormLinearProgram<DenseRow> a;
	NonStandardFormLinearProgram<SparseRow> b;
	if( false )
	{
		std::vector<double> tmp;
		sm.SolveLP( a, tmp );
		sm.SolveLP( b, tmp );
	}
}
TEST( SimplexMethod, compile_dual )
{
	SimplexMethod sm;
	NonStandardFormLinearProgram<DenseRow> a;
	NonStandardFormLinearProgram<SparseRow> b;
	std::vector<Equation<DenseRow>> qa;
	std::vector<Equation<SparseRow>> qb;
	if( false )
	{
		std::vector<double> tmp;
		sm.SolveWithNewConstraints( a, tmp, qa.begin(), qa.end() );
		sm.SolveWithNewConstraints( b, tmp, qb.begin(), qb.end() );
	}
}

TEST( SimplexMethod, DenseRow_multiply )
{
	DenseRow dr = { 1,2,3,4 };
	dr.multiply( 1.5 );
	EXPECT_DOUBLE_EQ( dr[0], 1.5 );
	EXPECT_DOUBLE_EQ( dr[1], 3 );
	EXPECT_DOUBLE_EQ( dr[2], 4.5 );
	EXPECT_DOUBLE_EQ( dr[3], 6 );
}
TEST( SimplexMethod, DenseRow_add )
{
	DenseRow a = { 1,2,3,4 };
	DenseRow b = { 2,3,4,5 };
	a.add( -2, b );
	EXPECT_DOUBLE_EQ( a[0], -3 );
	EXPECT_DOUBLE_EQ( a[1], -4 );
	EXPECT_DOUBLE_EQ( a[2], -5 );
	EXPECT_DOUBLE_EQ( a[3], -6 );
}
TEST( SimplexMethod, DenseRow_add_sparse )
{
	DenseRow a = { 1,2,3,4 };
	DenseRow b = { 0,3,4,0 };
	a.add( -2, b.toSparseRow() );
	EXPECT_DOUBLE_EQ( a[0], 1 );
	EXPECT_DOUBLE_EQ( a[1], -4 );
	EXPECT_DOUBLE_EQ( a[2], -5 );
	EXPECT_DOUBLE_EQ( a[3], 4 );
}
TEST( SimplexMethod, DenseRow_dot )
{
	DenseRow a = { 1,2,3,4 };
	DenseRow b = { 2,3,4,5 };
	const double ret = a.dot( b );
	EXPECT_DOUBLE_EQ( ret, 2 + 6 + 12 + 20 );
}
TEST( SimplexMethod, DenseRow_add_diff_size )
{
	DenseRow a = { 2,3 };
	DenseRow b = { 1,2,3,4 };
	a.add( 1, b );
	EXPECT_DOUBLE_EQ( a[0], 3 );
	EXPECT_DOUBLE_EQ( a[1], 5 );
	EXPECT_DOUBLE_EQ( a[2], 3 );
	EXPECT_DOUBLE_EQ( a[3], 4 );
}
TEST( SimplexMethod, DenseRow_dot_diff_size )
{
	DenseRow a = { 1,2,3,4 };
	DenseRow b = { 2,3 };
	const double ret = a.dot( b );
	EXPECT_DOUBLE_EQ( ret, 2 + 6 );
	EXPECT_DOUBLE_EQ( ret, b.dot( a ) );
}
TEST( SimplexMethod, hybrid_dot )
{
	DenseRow a = { 1,2,3,4 };
	SparseRow b = DenseRow( { 0,2,0,-1 } ).toSparseRow();
	const double x = a.dot( b );
	const double y = b.dot( a );
	EXPECT_DOUBLE_EQ( x, y );
	EXPECT_DOUBLE_EQ( x, 0 );
}
TEST( SimplexMethod, toSparseRow )
{
	SparseRow sr = DenseRow( { 1,2,3 } ).toSparseRow();
	ASSERT_EQ( sr.size(), 3 );
	EXPECT_DOUBLE_EQ( sr.get( 0 ).second, 1 );
	EXPECT_DOUBLE_EQ( sr.get( 1 ).second, 2 );
	EXPECT_DOUBLE_EQ( sr.get( 2 ).second, 3 );

	EXPECT_DOUBLE_EQ( sr.get( 0 ).first, 0 );
	EXPECT_DOUBLE_EQ( sr.get( 1 ).first, 1 );
	EXPECT_DOUBLE_EQ( sr.get( 2 ).first, 2 );
}
TEST( SimplexMethod, toSparseRow_zero )
{
	SparseRow sr = DenseRow( { 0,1,0,0,3,0 } ).toSparseRow();
	ASSERT_EQ( sr.size(), 2 );
	EXPECT_DOUBLE_EQ( sr.get( 0 ).second, 1 );
	EXPECT_DOUBLE_EQ( sr.get( 1 ).second, 3 );
	
	EXPECT_DOUBLE_EQ( sr.get( 0 ).first, 1 );
	EXPECT_DOUBLE_EQ( sr.get( 1 ).first, 4 );
}

TEST( SimplexMethod, SparseRow_index_const )
{
	DenseRow dr = { 1,0,3,4 };
	SparseRow sr = dr.toSparseRow();
	
	EXPECT_DOUBLE_EQ( sr[0], dr[0] );
	EXPECT_DOUBLE_EQ( sr[1], dr[1] );
	EXPECT_DOUBLE_EQ( sr[2], dr[2] );
	EXPECT_DOUBLE_EQ( sr[3], dr[3] );
}
TEST( SimplexMethod, SparseRow_index )
{
	SparseRow sr;
	sr[2] = 3;
	EXPECT_EQ( sr[2], 3 );
	sr[9] = 1;
	EXPECT_EQ( sr[9], 1 );
}
TEST( SimplexMethod, SparseRow_large_index_const )
{
	const int n = 200;
	std::mt19937 rng( 0 );
	std::uniform_int_distribution<int> randint( -4, 4 );
	
	DenseRow dr;
	dr.reserve( n );
	for( int i = 0; i < n; i++ )
		dr.emplace_back( randint( rng ) );
	SparseRow sr = dr.toSparseRow();
	for( int i = 0; i < n; i++ )
		ASSERT_DOUBLE_EQ( sr[i], dr[i] );
}
TEST( SimplexMethod, SparseRow_large_index )
{
	const int n = 50;
	std::mt19937 rng( 0 );
	std::uniform_int_distribution<int> randint( -4, 4 );
	std::uniform_int_distribution<int> randpos( 0, n - 1 );
	
	std::vector<std::pair<int, int>> input;
	input.reserve( n );
	for( int i = 0; i < n; i++ )
		input.emplace_back( randpos( rng ), randint( rng ) );

	std::vector<int> arr;
	arr.resize( n, 0 );
	SparseRow sr;
	for( auto [idx, val] : input )
	{
		sr[idx] = val;
		arr[idx] = val;
		for( int k = 0; k < n; k++ )
			ASSERT_DOUBLE_EQ( sr[k], arr[k] );
	}
}
TEST( SimplexMethod, SparseRow_build )
{
	std::vector<double> vec = { 0,1,2,0,3 };
	SparseRow sr( vec.begin(), vec.end() );
	ASSERT_EQ( sr.size(), 3 );
	EXPECT_EQ( sr.get( 0 ).second, 1 );
	EXPECT_EQ( sr.get( 1 ).second, 2 );
	EXPECT_EQ( sr.get( 2 ).second, 3 );
}
TEST( SimplexMethod, SparseRow_reduce )
{
	SparseRow sr1 = DenseRow( { 1,-2 } ).toSparseRow();
	SparseRow sr2 = DenseRow( { -1,2 } ).toSparseRow();
	sr1.add( 1, sr2 );
	sr1.reduce();
	EXPECT_EQ( sr1.size(), 0 );
}
TEST( SimplexMethod, SparseRow_multiply )
{
	SparseRow sr = DenseRow( { 1,2,3,4 } ).toSparseRow();
	sr.multiply( 1.5 );
	EXPECT_DOUBLE_EQ( sr[0], 1.5 );
	EXPECT_DOUBLE_EQ( sr[1], 3 );
	EXPECT_DOUBLE_EQ( sr[2], 4.5 );
	EXPECT_DOUBLE_EQ( sr[3], 6 );
}
TEST( SimplexMethod, SparseRow_check )
{
	SparseRow sr = DenseRow( { 1,2,3,4 } ).toSparseRow();
	EXPECT_TRUE( sr.check() );

	sr.resize( 2 );
	sr.set( 0, { 2,1 } );
	sr.set( 1, { 1,2 } );
	EXPECT_FALSE( sr.check() );
	
	sr.resize( 2 );
	sr.set( 0, { 3,1 } );
	sr.set( 1, { 3,2 } );
	EXPECT_FALSE( sr.check() );

	EXPECT_TRUE( SparseRow().check() );
}
TEST( SimplexMethod, SparseRow_check_empty )
{
	SparseRow sr;
	EXPECT_TRUE( sr.check() );
}
TEST( SimplexMethod, SparseRow_sort )
{
	SparseRow sr;
	sr.resize( 3 );
	sr.set( 0, { 2,1 } );
	sr.set( 1, { 1,2 } );
	sr.set( 2, { 3,3 } );
	sr.sort();
	EXPECT_DOUBLE_EQ( sr.get( 0 ).second, 2 );
	EXPECT_DOUBLE_EQ( sr.get( 1 ).second, 1 );
	EXPECT_DOUBLE_EQ( sr.get( 2 ).second, 3 );

	EXPECT_DOUBLE_EQ( sr.get( 0 ).first, 1 );
	EXPECT_DOUBLE_EQ( sr.get( 1 ).first, 2 );
	EXPECT_DOUBLE_EQ( sr.get( 2 ).first, 3 );
}
TEST( SimplexMethod, SparseRow_dot )
{
	SparseRow sr1 = DenseRow( { 0,2,4,0 } ).toSparseRow();
	SparseRow sr2 = DenseRow( { 0,0,8,1 } ).toSparseRow();
	const double ret = sr1.dot( sr2 );
	EXPECT_DOUBLE_EQ( ret, 32 );
}
TEST( SimplexMethod, SparseRow_add )
{
	SparseRow sr1 = DenseRow( { 0,2,4,0 } ).toSparseRow();
	SparseRow sr2 = DenseRow( { 0,0,8,1 } ).toSparseRow();
	sr1.add( 2, sr2 );
	ASSERT_EQ( sr1.size(), 3 );
	ASSERT_TRUE( sr1.check() );
	EXPECT_DOUBLE_EQ( sr1.get( 0 ).first, 1 );
	EXPECT_DOUBLE_EQ( sr1.get( 1 ).first, 2 );
	EXPECT_DOUBLE_EQ( sr1.get( 2 ).first, 3 );
	EXPECT_DOUBLE_EQ( sr1.get( 0 ).second, 2 );
	EXPECT_DOUBLE_EQ( sr1.get( 1 ).second, 20 );
	EXPECT_DOUBLE_EQ( sr1.get( 2 ).second, 2 );
}
TEST( SimplexMethod, SparseRow_add_randomtest )
{
	std::mt19937 rng( 0 );
	const int n_test = 100;
	const int n = 10;
	const int maxval = 3;

	std::uniform_int_distribution<int> randint( -maxval, maxval );
	const auto randomDenseRow = [&] ()->DenseRow
	{
		DenseRow dr;
		dr.reserve( n );
		for( int i = 0; i < n; i++ )
			dr.emplace_back( randint( rng ) );
		return dr;
	};
	for( int i = 0; i < n_test; ++i )
	{
		DenseRow dr1 = randomDenseRow();
		DenseRow dr2 = randomDenseRow();
		SparseRow sr1 = dr1.toSparseRow();
		SparseRow sr2 = dr2.toSparseRow();
		const int coef = randint( rng );
		sr1.add( coef, sr2 );
		for( int k = 0; k < n; ++k )
		{
			int val_sr = static_cast<int>( std::round( sr1[k] ) );
			int val_std = static_cast<int>( std::round( dr1[k] + dr2[k] * coef ) );
			ASSERT_EQ( val_sr, val_std );
		}
	}
}

TEST( SimplexMethod, Equation_reverse_relation )
{
	Equation<DenseRow> a;
	a.get_relation() = tRelation::kEQ;
	a.reverse_relation();
	EXPECT_EQ( a.get_relation(), tRelation::kEQ );

	a.get_relation() = tRelation::kLE;
	a.reverse_relation();
	EXPECT_EQ( a.get_relation(), tRelation::kGE );

	a.get_relation() = tRelation::kGE;
	a.reverse_relation();
	EXPECT_EQ( a.get_relation(), tRelation::kLE );
}
TEST( SimplexMethod, Equation_substitute )
{
	{
		Equation<DenseRow> a;
		a.get_vector() = { 2,3 };
		a.get_rhs() = 9;
		a.substitute( 1, 3 );
		EXPECT_DOUBLE_EQ( a.get_vector()[1], 0 );
		EXPECT_DOUBLE_EQ( a.get_rhs(), 0 );
	}
	{
		Equation<SparseRow> a;
		a.get_vector() = DenseRow( { 2,3 } ).toSparseRow();
		a.get_rhs() = 9;
		a.substitute( 1, 3 );
		EXPECT_DOUBLE_EQ( a.get_vector()[1], 0 );
		EXPECT_DOUBLE_EQ( a.get_rhs(), 0 );
	}
}
TEST( SimplexMethod, Equation_satisfication )
{
	Equation<DenseRow> dr1( { 2,0,3,4 }, tRelation::kEQ, 18 );//Y
	Equation<DenseRow> dr2( { 2,0,3,4 }, tRelation::kLE, 19 );//Y
	Equation<DenseRow> dr3( { 2,0,3,4 }, tRelation::kGE, 19 );//N
	DenseRow result = { 1,2,0,4 };

	EXPECT_TRUE( dr1.isSatisfied( result ) );
	EXPECT_TRUE( dr2.isSatisfied( result ) );
	EXPECT_FALSE( dr3.isSatisfied( result ) );
	
	Equation<SparseRow> sr1( dr1.get_vector().toSparseRow(), tRelation::kEQ, 18 );//Y
	Equation<SparseRow> sr2( dr2.get_vector().toSparseRow(), tRelation::kLE, 19 );//Y
	Equation<SparseRow> sr3( dr3.get_vector().toSparseRow(), tRelation::kGE, 19 );//N
	EXPECT_TRUE( sr1.isSatisfied( result.toSparseRow() ) );
	EXPECT_TRUE( sr2.isSatisfied( result.toSparseRow() ) );
	EXPECT_FALSE( sr3.isSatisfied( result.toSparseRow() ) );
}

TEST( SimplexMethod, NonStandardFormLinearProgram_compile )
{
	NonStandardFormLinearProgram<SparseRow> x;
	x.Substitute( 0, 1 );

	NonStandardFormLinearProgram<DenseRow> y;
	y.Substitute( 0, 1 );
}

TEST( SimplexMethod, stdform1 )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;
	
	const int n = 2;
	input.lb.resize( n, 0 );
	input.isMaximization = true;
	input.objectivefunc = { 3,1 };
	input.emplace_back( std::make_tuple( DenseRow( { 2,1 } ), tRelation::kLE, 8 ) );
	input.emplace_back( std::make_tuple( DenseRow( { 2,3 } ), tRelation::kLE, 12 ) );
	
	const double best = 12;
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	
	ASSERT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, best );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), best );

	ASSERT_EQ( result.size(), 2 );
	EXPECT_DOUBLE_EQ( result[0], 4 );
	EXPECT_DOUBLE_EQ( result[1], 0 );
}
//from The two-phase simplex method::case 2
TEST( SimplexMethod, case_cleanbasis )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 4,5 } );
	input.emplace_back( DenseRow( { 2,3 } ), tRelation::kLE, 6 );
	input.emplace_back( DenseRow( { 3,1 } ), tRelation::kGE, 3 );
	input.lb = { 0,0 };
	
	const double best = 12;
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, best );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), best );

	EXPECT_DOUBLE_EQ( result[0], 3 );
	EXPECT_DOUBLE_EQ( result[1], 0 );
}
//from The two-phase simplex method::case 3
TEST( SimplexMethod, case_noncleanbasis )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 40,10,0,0,7,14 } );
	input.emplace_back( DenseRow( { 1,-1,0,0,2,0 } ), tRelation::kEQ, 0 );
	input.emplace_back( DenseRow( { -2,1,0,0,-2,0 } ), tRelation::kEQ, 0 );
	input.emplace_back( DenseRow( { 1,0,1,0,1,-1 } ), tRelation::kEQ, 3 );
	input.emplace_back( DenseRow( { 0,1,1,1,2,1 } ), tRelation::kEQ, 4 );
	input.lb.resize( 6, 0 );
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	const double best = 9;
	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, best );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), best );

	ASSERT_EQ( result.size(), 6 );
	EXPECT_DOUBLE_EQ( result[0], 0 );
	EXPECT_DOUBLE_EQ( result[1], 2.0/3 );
	EXPECT_DOUBLE_EQ( result[2], 8.0/3 );
	EXPECT_DOUBLE_EQ( result[3], 0 );
	EXPECT_DOUBLE_EQ( result[4], 1.0/3 );
	EXPECT_DOUBLE_EQ( result[5], 0 );
}
//from The two-phase simplex method:: case 1
TEST( SimplexMethod, case_nosolution )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 2,3 } );
	input.emplace_back( DenseRow( { 0.5,0.25 } ), tRelation::kLE, 4 );
	input.emplace_back( DenseRow( { 1,3 } ), tRelation::kGE, 36 );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kEQ, 10 );
	input.lb.resize( 2, 0 );
	
	auto [r, ofv] = sm.SolveLP( input, result );
	EXPECT_EQ( r, SimplexMethod::tError::kInfeasible );
}
TEST( SimplexMethod, shift )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;
	
	const int n = 2;
	input.lb = { 3,5 };//x>=3, y>=5
	input.isMaximization = true;
	input.objectivefunc = { 1,-1 };//max(x-y)
	input.emplace_back( DenseRow( { 1,0 } ), tRelation::kLE, 8 );//x<=8
	input.emplace_back( DenseRow( { 0,1 } ), tRelation::kLE, 12 );//y<=12
	
	const double best = 8 - 5;
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	ASSERT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, best );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), best );

	ASSERT_EQ( result.size(), 2 );
	EXPECT_DOUBLE_EQ( result[0], 8 );
	EXPECT_DOUBLE_EQ( result[1], 5 );
}
TEST( SimplexMethod, R_domain )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;
	
	const int n = 2;
	input.lb = { -INF,-INF };
	input.isMaximization = true;
	input.objectivefunc = { 1,-1 };//max(x-y)
	input.emplace_back( DenseRow( { 1,0 } ), tRelation::kLE, 8 );//x<=8
	input.emplace_back( DenseRow( { 0,1 } ), tRelation::kLE, 12 );//y<=12
	input.emplace_back( DenseRow( { 1,0 } ), tRelation::kGE, 3 );//x>=3
	input.emplace_back( DenseRow( { 0,1 } ), tRelation::kGE, 5 );//y>=5
	
	const double best = 8 - 5;
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	ASSERT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, best );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), best );

	ASSERT_EQ( result.size(), 2 );
	EXPECT_DOUBLE_EQ( result[0], 8 );
	EXPECT_DOUBLE_EQ( result[1], 5 );
}
TEST( SimplexMethod, min2max )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;
	
	const int n = 2;
	input.lb.resize( n, 0 );
	input.isMaximization = false;
	input.objectivefunc = { 1,1 };
	input.emplace_back( DenseRow( { 1,0 } ), tRelation::kGE, 5 );
	input.emplace_back( DenseRow( { 0,1 } ), tRelation::kGE, 10 );
	
	const double best = 10 + 5;
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	ASSERT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, best );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), best );

	ASSERT_EQ( result.size(), 2 );
	EXPECT_DOUBLE_EQ( result[0], 5 );
	EXPECT_DOUBLE_EQ( result[1], 10 );
}
TEST( SimplexMethod, neg_of )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;
	
	const int n = 1;
	input.lb.resize( n, 0 );
	input.isMaximization = true;
	input.objectivefunc = { -1 };
	input.emplace_back( DenseRow( { 1 } ), tRelation::kGE, 5 );
	
	const double best = -5;
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	ASSERT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, best );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), best );

	ASSERT_EQ( result.size(), 1 );
	EXPECT_DOUBLE_EQ( result[0], 5 );
}
TEST( SimplexMethod, trivial_equation )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;
	
	const int n = 1;
	input.lb.resize( n, 0 );
	input.isMaximization = true;
	input.objectivefunc = { 1 };
	input.emplace_back( DenseRow( { 0 } ), tRelation::kGE, 0 );
	
	auto before = input;
	ASSERT_NO_THROW( sm.SolveLP( input, result ) );
}
TEST( SimplexMethod, R_transform_empty_equation )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;
	
	const int n = 2;
	input.lb = { -INF,-INF };
	input.isMaximization = false;
	input.objectivefunc = { -1,-1 };
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kEQ, 2 );
	input.emplace_back( DenseRow( { 1,2 } ), tRelation::kLE, 1 );
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	
	ASSERT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_TRUE( before.CheckLowerBound( result.begin(), result.end() ) );
	EXPECT_TRUE( before.CheckEquation( result.begin(), result.end() ) );
}
TEST( SimplexMethod, bug_case_noncleanbasis )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;
	
	const int n = 1;
	input.lb = { 0 };
	input.isMaximization = true;
	input.objectivefunc = { -1 };
	input.emplace_back( DenseRow( { 1 } ), tRelation::kLE, 1 );
	input.emplace_back( DenseRow( { 1 } ), tRelation::kEQ, 1 );
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	
	ASSERT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_TRUE( before.CheckLowerBound( result.begin(), result.end() ) );
	EXPECT_TRUE( before.CheckEquation( result.begin(), result.end() ) );
}

TEST( SimplexMethod, bug_dead_loop )
{
	SimplexMethod sm;
	sm.SetPerturbation( true );//solution for this bug
	NonStandardFormLinearProgram<DenseRow> dr_input;
	NonStandardFormLinearProgram<SparseRow> sr_input;
	std::vector<double> dr_result, sr_result;

	//dense
	dr_input.isMaximization = true;
	dr_input.objectivefunc = { -5,0,1,-1,4 };
	dr_input.emplace_back( DenseRow( { 4,5,-3,4,3 } ), tRelation::kGE, 6 );
	dr_input.emplace_back( DenseRow( { -1,0,1,3,-3 } ), tRelation::kLE, -6 );
	dr_input.emplace_back( DenseRow( { 2,-2,0,-2,-2 } ), tRelation::kEQ, 9 );
	dr_input.emplace_back( DenseRow( { -4,3,1,-2,-4 } ), tRelation::kLE, -4 );
	dr_input.emplace_back( DenseRow( { -4,3,-5,5,1 } ), tRelation::kLE, 1 );
	dr_input.lb = { 0,0,-INF,-1,0 };

	//sparse
	sr_input.isMaximization = dr_input.isMaximization;
	sr_input.objectivefunc = dr_input.objectivefunc.toSparseRow();
	for( auto& e : dr_input )
		sr_input.emplace_back( e.get_vector().toSparseRow(), e.get_relation(), e.get_rhs() );
	sr_input.lb = dr_input.lb;

	//run
	auto dr_before = dr_input;
	auto [r1, ofv1] = sm.SolveLP( dr_input, dr_result );

	auto sr_before = sr_input;
	auto [r2, ofv2] = sm.SolveLP( sr_input, sr_result );

	EXPECT_EQ( r1, r2 );
}
TEST( SimplexMethod, bug_different_result )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> dr_input;
	NonStandardFormLinearProgram<SparseRow> sr_input;
	std::vector<double> dr_result, sr_result;

	//dense
	dr_input.isMaximization = false;
	dr_input.objectivefunc = { 5,-1,2,-2 };
	dr_input.emplace_back( DenseRow( { -1,0,-4,5 } ), tRelation::kLE, -1 );
	dr_input.emplace_back( DenseRow( { -2,1,2,4 } ), tRelation::kEQ, -12 );
	dr_input.emplace_back( DenseRow( { -3,-5,0,-4 } ), tRelation::kEQ, 4 );
	dr_input.emplace_back( DenseRow( { -2,1,2,4 } ), tRelation::kEQ, 10 );
	dr_input.lb = { -1,-INF,0,0 };

	//sparse
	sr_input.isMaximization = dr_input.isMaximization;
	sr_input.objectivefunc = dr_input.objectivefunc.toSparseRow();
	for( auto& e : dr_input )
		sr_input.emplace_back( e.get_vector().toSparseRow(), e.get_relation(), e.get_rhs() );
	sr_input.lb = dr_input.lb;

	//run
	auto dr_before = dr_input;
	auto [r1, ofv1] = sm.SolveLP( dr_input, dr_result );

	auto sr_before = sr_input;
	auto [r2, ofv2] = sm.SolveLP( sr_input, sr_result );

	EXPECT_EQ( r1, r2 );
}
TEST( SimplexMethod, bug_small )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = false;
	input.objectivefunc = DenseRow( { 1,1 } );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kGE, 10 );
	input.lb = { 0,0 };
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, 10 );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 10 );
}
TEST( SimplexMethod, bug_recover )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	NonStandardFormLinearProgram<SparseRow> sr_input;
	std::vector<double> result, sr_result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { -5,-3,-5,-2,0 } );
	input.emplace_back( DenseRow( { 2,0,3,-5,5 } ), tRelation::kGE, -10 );
	input.emplace_back( DenseRow( { 4,-4,-2,0,-1 } ), tRelation::kEQ, 12 );
	input.emplace_back( DenseRow( { -3,0,1,-2,1 } ), tRelation::kLE, -6 );
	input.emplace_back( DenseRow( { 2,-1,-1,2,-2 } ), tRelation::kGE, 5 );
	input.emplace_back( DenseRow( { 3,-5,1,0,-2 } ), tRelation::kGE, -9 );
	input.emplace_back( DenseRow( { 0,-4,4,2,3 } ), tRelation::kEQ, 1 );
	input.lb = { 1,-INF,-INF,-1,-INF };

	
	sr_input.isMaximization = input.isMaximization;
	sr_input.objectivefunc = input.objectivefunc.toSparseRow();
	for( auto& e : input )
		sr_input.emplace_back( e.get_vector().toSparseRow(), e.get_relation(), e.get_rhs() );
	sr_input.lb = input.lb;
	
	auto before = input;
	auto [r1, ofv1] = sm.SolveLP( input, result );
	auto [r2, ofv2] = sm.SolveLP( sr_input, sr_result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r1, r2 );
	//EXPECT_DOUBLE_EQ( ofv, 10 );
	//EXPECT_TRUE( before.CheckLowerBound( dr ) );
	//EXPECT_TRUE( before.CheckEquation( dr ) );
	//EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 10 );
}

TEST( SimplexMethod, SolveWithNewConstraints_infeasible )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 1,1 } );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kLE, 10 );
	input.lb = { 0,0 };
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, 10 );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 10 );

	Equation<DenseRow> eq( DenseRow( { 1,0 } ), tRelation::kGE, 15 );
	auto [r2, ofv2] = sm.SolveWithNewConstraints( input, result, eq );
	EXPECT_EQ( r2, SimplexMethod::tError::kInfeasible );
}
//fix by add basis transform
TEST( SimplexMethod, SolveWithNewConstraints_le )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 1,1 } );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kLE, 10 );
	input.lb = { 0,0 };
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, 10 );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 10 );

	Equation<DenseRow> eq( DenseRow( { 1,1 } ), tRelation::kLE, 5 );
	auto [r2, ofv2] = sm.SolveWithNewConstraints( input, result, eq );
	EXPECT_EQ( r2, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv2, 5 );
}
TEST( SimplexMethod, SolveWithNewConstraints_ge )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 1,1 } );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kLE, 10 );
	input.lb = { 0,0 };
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, 10 );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 10 );

	Equation<DenseRow> eq( DenseRow( { 0,1 } ), tRelation::kGE, 3 );
	auto [r2, ofv2] = sm.SolveWithNewConstraints( input, result, eq );
	EXPECT_EQ( r2, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv2, 10 );
}
TEST( SimplexMethod, SolveWithNewConstraints_multiple )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 1,1 } );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kLE, 10 );
	input.lb = { 0,0 };
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, 10 );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 10 );

	Equation<DenseRow> eq( DenseRow( { 1,1 } ), tRelation::kLE, 5 );
	auto [r2, ofv2] = sm.SolveWithNewConstraints( input, result, eq );
	EXPECT_EQ( r2, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv2, 5 );

	Equation<DenseRow> eq2( DenseRow( { 1,1 } ), tRelation::kLE, 3 );
	auto [r3, ofv3] = sm.SolveWithNewConstraints( input, result, eq2 );
	EXPECT_EQ( r3, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv3, 3 );
}
TEST( SimplexMethod, SolveWithNewConstraints_shift )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 1,1 } );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kLE, 10 );
	input.lb = { -1,1 };
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, 10 );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 10 );

	Equation<DenseRow> eq( DenseRow( { 1,0 } ), tRelation::kLE, -1 );
	auto [r2, ofv2] = sm.SolveWithNewConstraints( input, result, eq );
	EXPECT_EQ( r2, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( result[0], -1 );
}
TEST( SimplexMethod, SolveWithNewConstraints_recover )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 1,1 } );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kEQ, 5 );
	input.lb = { -INF,0 };
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, 5 );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 5 );

	Equation<DenseRow> eq( DenseRow( { 2,1 } ), tRelation::kLE, 8 );
	auto [r2, ofv2] = sm.SolveWithNewConstraints( input, result, eq );
	EXPECT_EQ( r2, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv2, 5 );
}
TEST( SimplexMethod, SolveWithNewConstraints_min )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input;
	std::vector<double> result;

	input.isMaximization = false;
	input.objectivefunc = DenseRow( { 1,1 } );
	input.emplace_back( DenseRow( { 1,1 } ), tRelation::kGE, 5 );
	input.lb = { 0,0 };
	
	auto before = input;
	auto [r, ofv] = sm.SolveLP( input, result );
	DenseRow dr( result.begin(), result.end() );

	EXPECT_EQ( r, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv, 5 );
	EXPECT_TRUE( before.CheckLowerBound( dr ) );
	EXPECT_TRUE( before.CheckEquation( dr ) );
	EXPECT_DOUBLE_EQ( before.CalcOFV( dr ), 5 );

	Equation<DenseRow> eq( DenseRow( { 2,1 } ), tRelation::kLE, 8 );
	auto [r2, ofv2] = sm.SolveWithNewConstraints( input, result, eq );
	EXPECT_EQ( r2, SimplexMethod::tError::kOptimum );
	EXPECT_DOUBLE_EQ( ofv2, 5 );
}

TEST( SimplexMethod, SolveWithNewConstraints_compare_with_solveLP )
{
	SimplexMethod sm;
	sm.SetPerturbation( false );
	NonStandardFormLinearProgram<DenseRow> input, std_input;
	std::vector<double> result,std_result;

	Equation<DenseRow> eq1( DenseRow( { 1,1,1,1 } ), tRelation::kLE, 50 );
	Equation<DenseRow> eq2( DenseRow( { -2,-3,3,2 } ), tRelation::kLE, 15 );
	Equation<DenseRow> eq3( DenseRow( { 0,1,0,4 } ), tRelation::kGE, 9 );
	Equation<DenseRow> eq4( DenseRow( { 4,2,4,1 } ), tRelation::kGE, 10 );
	Equation<DenseRow> eq5( DenseRow( { 8,9,7,0 } ), tRelation::kEQ, 7 );

	input.isMaximization = true;
	input.objectivefunc = DenseRow( { 1,2,3,4 } );
	input.emplace_back( eq1 );
	input.emplace_back( eq2 );
	input.emplace_back( eq3 );
	//input.lb = { -1,0,1,-INF };
	input.lb = { -1,0,0,0 };

	std_input = input;
	std_input.emplace_back( eq4 );
	std_input.emplace_back( eq5 );

	auto p0 = std_input;
	auto [r0, ofv] = sm.SolveLP( std_input, std_result );

	sm.Clear();
	auto p1 = input;
	auto [r1, ofv1] = sm.SolveLP( input, result );
	DenseRow ans1( result.begin(), result.end() );
	auto [r2, ofv2] = sm.SolveWithNewConstraints( input, result, eq4 );
	DenseRow ans2( result.begin(), result.end() );
	auto [r3, ofv3] = sm.SolveWithNewConstraints( input, result, eq5 );
	DenseRow ans3( result.begin(), result.end() );
	EXPECT_EQ( r0, SimplexMethod::tError::kOptimum );
	EXPECT_EQ( r1, SimplexMethod::tError::kOptimum );
	EXPECT_EQ( r2, SimplexMethod::tError::kOptimum );
	EXPECT_EQ( r3, SimplexMethod::tError::kOptimum );

	EXPECT_TRUE( p0.CheckEquation( DenseRow( std_result.begin(), std_result.end() ) ) );
	EXPECT_TRUE( p1.CheckEquation( ans1 ) );
	EXPECT_TRUE( p1.CheckEquation( ans2 ) );
	EXPECT_TRUE( eq4.isSatisfied( ans2 ) );
	EXPECT_TRUE( p1.CheckEquation( ans3 ) );
	EXPECT_TRUE( eq4.isSatisfied( ans3 ) );
	EXPECT_TRUE( eq5.isSatisfied( ans3 ) );
	EXPECT_NEAR( ofv, ofv3, Util::eps );
}

TEST( SimplexMethod, performancetest )
{
#ifdef _DEBUG
	return;
#endif
	SimplexMethod sm;
	sm.SetPerturbation( true );
	NonStandardFormLinearProgram<SparseRow> sr_input;
	std::vector<double> sr_result;

	//1k,1k,0.1k=5s
	const int n = 1000;
	const int m = 1000;
	const int avgcell = 10;
	const double maxprecision = 100;

	std::mt19937 rng( 0 );
	std::uniform_real_distribution<double> randval( 1, 10 );
	std::uniform_real_distribution<double> randrhs( 1, 100 );

	const auto randvector = [&] ( const int validcell )->SparseRow
	{
		DenseRow dr;
		dr.resize( n, 0 );
		std::vector<int> arr;
		arr.reserve( n );
		for( int i = 0; i < n; ++i )
			arr.emplace_back( i );
		std::shuffle( arr.begin(), arr.end(), rng );
		for( int i = 0; i < validcell; ++i )
			dr[arr[i]] = std::round( randval( rng ) * maxprecision ) / maxprecision;
		return dr.toSparseRow();
	};
	const auto randomEquation = [&] ( const int validcell )->Equation<SparseRow>
	{
		Equation<SparseRow> eq;
		eq.get_vector() = randvector( validcell );
		eq.get_relation() = tRelation::kLE;
		eq.get_rhs() = std::round( randrhs( rng ) * maxprecision ) / maxprecision;
		return eq;
	};

	sr_input.isMaximization = true;
	sr_input.objectivefunc = randvector( n );
	sr_input.lb.resize( n, 0 );
	for( int j = 0; j < m; j++ )
		sr_input.emplace_back( randomEquation( avgcell ) );

	auto sr_before = sr_input;
	auto [r, ofv] = sm.SolveLP( sr_input, sr_result );
	const int tot = sm.GetTotalIteration();
	const double t = sm.GetTimeAsSecond();
	printf( "n=%d,m=%d,tot=%d,t=%.1lf,p1=%d,p2=%d,cell=%d\n", n, m, tot, t, sm.GetPhase1Iteration(), sm.GetPhase2Iteration(), avgcell * m + n );
	
	EXPECT_TRUE( sr_before.CheckLowerBound( sr_result.begin(), sr_result.end() ) );
	EXPECT_TRUE( sr_before.CheckEquation( sr_result.begin(), sr_result.end() ) );
	EXPECT_NEAR( sr_before.CalcOFV( sr_result.begin(), sr_result.end() ), ofv, Util::eps );
}

//for bug detect, skip
TEST( SimplexMethod, randomtest_denserow_sparserow )
{
#ifdef _DEBUG
	return;
#endif
	const int n_test = 10000;
	const int maxval = 5;
	const int maxrhs = 15;
	std::mt19937 rng( 0 );
	std::uniform_int_distribution<int> randn( 1, 7 );
	std::uniform_int_distribution<int> randm( 1, 30 );
	std::uniform_int_distribution<int> randmaxmin( 0, 1 );
	std::uniform_int_distribution<int> randint( -maxval, maxval );
	std::uniform_int_distribution<int> randrhs( -maxrhs, maxrhs );
	const auto randomDenseRow = [&]( const int n )->DenseRow
	{
		DenseRow dr;
		dr.reserve( n );
		for( int i = 0; i < n; i++ )
			dr.emplace_back( randint( rng ) );
		return dr;
	};
	const auto randRelation = [&rng] ()->tRelation
	{
		std::uniform_int_distribution<int> randx( 1, 3 );
		switch( randx( rng ) )
		{
		case 1:
			return tRelation::kEQ;
		case 2:
			return tRelation::kGE;
		case 3:
			return tRelation::kLE;
		default:
			assert( 0 );
			break;
		}
		assert( 0 );
		return tRelation::kEQ;
	};
	const auto randomEquation = [&]( const int n )->Equation<DenseRow>
	{
		Equation<DenseRow> eq;
		eq.get_vector() = randomDenseRow( n );
		eq.get_relation() = randRelation();
		eq.get_rhs() = randrhs( rng );
		return eq;
	};
	const auto randomLowerBound = [&] ()->double
	{
		std::uniform_int_distribution<int> randx( -1, 2 );
		int val = randx( rng );
		if( val == 2 )
			return -INF;
		else
			return val;
	};
	
	for( int i = 0; i < n_test; i++ )
	{
		const int n = randn( rng );
		const int m = randm( rng );

		SimplexMethod sm;
		sm.SetPerturbation( true );
		NonStandardFormLinearProgram<DenseRow> dr_input;
		NonStandardFormLinearProgram<SparseRow> sr_input;
		std::vector<double> dr_result, sr_result;

		//dense
		dr_input.isMaximization = randmaxmin( rng );
		dr_input.objectivefunc = randomDenseRow( n );
		for( int j = 0; j < m; j++ )
			dr_input.emplace_back( randomEquation( n ) );
		dr_input.lb.resize( n, 0 );
		for( int j = 0; j < n; j++ )
			dr_input.lb[j] = randomLowerBound();
		
		//sparse
		sr_input.isMaximization = dr_input.isMaximization;
		sr_input.objectivefunc = dr_input.objectivefunc.toSparseRow();
		for( auto& e : dr_input )
			sr_input.emplace_back( e.get_vector().toSparseRow(), e.get_relation(), e.get_rhs() );
		sr_input.lb = dr_input.lb;

		//run
		auto dr_before = dr_input;
		sm.SetSeed( 0 );
		auto [r1, ofv1] = sm.SolveLP( dr_input, dr_result );

		auto sr_before = sr_input;
		sm.SetSeed( 0 );
		auto [r2, ofv2] = sm.SolveLP( sr_input, sr_result );
		
		if( i == -1 )
		{
			std::cout << i << '\n';
			std::cout << dr_before.toString() << '\n';
			//sm.SolveLP( dr_before, dr_result );
			//sm.SolveLP( sr_before, sr_result );
		}

		//final check
		ASSERT_EQ( r1, r2 );
		if( r1 != SimplexMethod::tError::kOptimum || r2 != SimplexMethod::tError::kOptimum )
			continue;
		ASSERT_NEAR( ofv1, ofv2, Util::eps );
		
		ASSERT_TRUE( dr_before.CheckLowerBound( dr_result.begin(), dr_result.end() ) );
		ASSERT_TRUE( sr_before.CheckLowerBound( sr_result.begin(), sr_result.end() ) );
		ASSERT_TRUE( dr_before.CheckEquation( dr_result.begin(), dr_result.end() ) );
		ASSERT_TRUE( sr_before.CheckEquation( sr_result.begin(), sr_result.end() ) );
		ASSERT_NEAR( dr_before.CalcOFV( dr_result.begin(), dr_result.end() ), ofv1, Util::eps );
		ASSERT_NEAR( sr_before.CalcOFV( sr_result.begin(), sr_result.end() ), ofv2, Util::eps );

		if( !sm.GetPerturbation() )//some how error cause diff solution
		{
			ASSERT_EQ( dr_result.size(), sr_result.size() );
			for( int j = 0; j < n; j++ )
				ASSERT_NEAR( dr_result[j], sr_result[j], Util::eps );
		}
	}
}

TEST( SimplexMethod, randomtest_SolveWithNewConstraints )
{
#ifdef _DEBUG
	return;
#endif
	const int n_test = 5000;
	const int maxval = 5;
	const int maxrhs = 15;
	const int boundval = 100;
	std::mt19937 rng( 0 );
	std::uniform_int_distribution<int> randn( 1, 15 );
	std::uniform_int_distribution<int> randm( 1, 50 );
	std::uniform_int_distribution<int> randmaxmin( 0, 1 );
	std::uniform_int_distribution<int> randint( -maxval, maxval );
	std::uniform_int_distribution<int> randrhs( -maxrhs, maxrhs );
	const auto randomDenseRow = [&]( const int n )->DenseRow
	{
		DenseRow dr;
		dr.reserve( n );
		for( int i = 0; i < n; i++ )
			dr.emplace_back( randint( rng ) );
		return dr;
	};
	const auto randRelation = [&rng] ()->tRelation
	{
		std::uniform_int_distribution<int> randx( 1, 3 );
		switch( randx( rng ) )
		{
		case 1:
			return tRelation::kEQ;
		case 2:
			return tRelation::kGE;
		case 3:
			return tRelation::kLE;
		default:
			assert( 0 );
			break;
		}
		assert( 0 );
		return tRelation::kEQ;
	};
	const auto randomEquation = [&]( const int n )->Equation<DenseRow>
	{
		Equation<DenseRow> eq;
		eq.get_vector() = randomDenseRow( n );
		eq.get_relation() = randRelation();
		eq.get_rhs() = randrhs( rng );
		return eq;
	};
	const auto randomLowerBound = [&] ()->double
	{
		std::uniform_int_distribution<int> randx( -1, 2 );
		int val = randx( rng );
		if( val == 2 )
			return -INF;
		else
			return val;
	};
	
	for( int i = 0; i < n_test; i++ )
	{
		const int n = randn( rng );
		const int m = randm( rng );

		SimplexMethod sm_dr, sm_sr;
		sm_dr.SetPerturbation( true );
		sm_sr.SetPerturbation( true );
		NonStandardFormLinearProgram<DenseRow> dr_input;
		NonStandardFormLinearProgram<SparseRow> sr_input;
		std::vector<double> dr_result, sr_result;
		std::vector<Equation<DenseRow>> eq_list;
		eq_list.reserve( m );
		for( int j = 0; j < m; j++ )
			eq_list.emplace_back( randomEquation( n ) );

		//dense
		dr_input.isMaximization = randmaxmin( rng );
		dr_input.objectivefunc = randomDenseRow( n );
		for( int j = 0; j < n; j++ )//make sure a valid initial solution
		{
			Equation<DenseRow> eq;
			eq.get_rhs() = boundval;
			eq.get_relation() = tRelation::kLE;
			eq.get_vector().resize( n, 0 );
			eq.get_vector()[j] = 1;
			dr_input.emplace_back( eq );

			eq.get_relation() = tRelation::kGE;
			eq.get_rhs() = -boundval;
			dr_input.emplace_back( eq );
		}
		dr_input.lb.resize( n, 0 );
		for( int j = 0; j < n; j++ )
			dr_input.lb[j] = randomLowerBound();
		
		//sparse
		sr_input.isMaximization = dr_input.isMaximization;
		sr_input.objectivefunc = dr_input.objectivefunc.toSparseRow();
		for( auto& e : dr_input )
			sr_input.emplace_back( e.get_vector().toSparseRow(), e.get_relation(), e.get_rhs() );
		sr_input.lb = dr_input.lb;

		//run
		sm_sr.SetSeed( 0 );
		auto [r0, dummy] = sm_sr.SolveLP( sr_input, sr_result );
		assert( r0 == SimplexMethod::tError::kOptimum );
		int idx = 0;
		for( auto& eq : eq_list )
		{
			++idx;

			dr_input.emplace_back( eq );
			auto dr_before = dr_input;
			sm_dr.SetSeed( 0 );
			sm_dr.Clear();
			auto [r1, ofv1] = sm_dr.SolveLP( dr_input, dr_result );

			auto sr_before = sr_input;
			sm_sr.SetSeed( 0 );
			auto [r2, ofv2] = sm_sr.SolveWithNewConstraints( sr_input, sr_result, eq.toSparseEquation() );

			if( r1 != r2 )
				std::cout << i << ' ' << idx << '\n';

			//final check
			ASSERT_EQ( r1, r2 );
			if( r1 != SimplexMethod::tError::kOptimum || r2 != SimplexMethod::tError::kOptimum )
				break;
			ASSERT_NEAR( ofv1, ofv2, Util::eps );
			if (!dr_before.CheckEquation( dr_result.begin(), dr_result.end() ))
				std::cout << i << ' ' << idx << '\n';
			ASSERT_TRUE( dr_before.CheckLowerBound( dr_result.begin(), dr_result.end() ) );
			ASSERT_TRUE( dr_before.CheckLowerBound( sr_result.begin(), sr_result.end() ) );
			ASSERT_TRUE( dr_before.CheckEquation( dr_result.begin(), dr_result.end() ) );
			ASSERT_TRUE( dr_before.CheckEquation( sr_result.begin(), sr_result.end() ) );
			ASSERT_NEAR( dr_before.CalcOFV( dr_result.begin(), dr_result.end() ), ofv1, Util::eps );
			ASSERT_NEAR( dr_before.CalcOFV( sr_result.begin(), sr_result.end() ), ofv2, Util::eps );

			dr_input = dr_before;
		}
	}
}
