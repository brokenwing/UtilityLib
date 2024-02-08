#include "pch.h"
#include "MixedIntegerLinearProgram.h"

using namespace Util::ORtool;

TEST( MixedIntegerLinearProgram, compile )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;
	milp.SolveMILP( prob, result );
}

TEST( MixedIntegerLinearProgram, easy )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;

	const int n = 3;
	prob.lb.resize( n, 0 );
	prob.isMaximization = true;
	prob.objectivefunc = DenseRow( {1,2,3} ).toSparseRow();
	prob.int_range.resize( n );
	for( int i = 0; i < n; i++ )
		prob.int_range[i] = { 0,3 };
	for( int i = 0; i < n; i++ )
	{
		SparseRow sr;
		sr[i] = 1;
		prob.emplace_back( Equation<SparseRow>( sr, tRelation::kLE, prob.int_range[i].value().second ) );
	}
	prob.emplace_back( Equation<DenseRow>( { 1,1,1 }, tRelation::kLE, 3 ).toSparseEquation() );

	milp.SetMaxIteration( 100 );
	auto [r, ofv] = milp.SolveMILP( prob, result );

	EXPECT_EQ( r, MixedIntegerLinearProgramSolver::tError::kSuc );
	EXPECT_NEAR( ofv, 9, Util::eps );
	ASSERT_EQ( result.size(), 3 );
	EXPECT_DOUBLE_EQ( result[0], 0 );
	EXPECT_DOUBLE_EQ( result[1], 0 );
	EXPECT_NEAR( result[2], 3, Util::eps );
}
TEST( MixedIntegerLinearProgram, easy2 )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;

	const int n = 3;
	prob.lb.resize( n, 0 );
	prob.isMaximization = true;
	prob.objectivefunc = DenseRow( {1,2,3} ).toSparseRow();
	prob.int_range.resize( n );
	for( int i = 0; i < n; i++ )
		prob.int_range[i] = { 0,3 };
	for( int i = 0; i < n; i++ )
	{
		SparseRow sr;
		sr[i] = 1;
		prob.emplace_back( Equation<SparseRow>( sr, tRelation::kLE, prob.int_range[i].value().second ) );
	}
	prob.emplace_back( Equation<DenseRow>( { 1,1,1 }, tRelation::kLE, 2.5 ).toSparseEquation() );

	milp.SetMaxIteration( 100 );
	milp.SetTimelimit( 3600 );
	auto [r, ofv] = milp.SolveMILP( prob, result );

	EXPECT_EQ( r, MixedIntegerLinearProgramSolver::tError::kSuc );
	EXPECT_DOUBLE_EQ( ofv, 6 );
	ASSERT_EQ( result.size(), 3 );
	EXPECT_DOUBLE_EQ( result[0], 0 );
	EXPECT_DOUBLE_EQ( result[1], 0 );
	EXPECT_DOUBLE_EQ( result[2], 2 );
}
