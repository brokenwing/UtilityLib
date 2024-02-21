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
	EXPECT_NEAR( ofv, 6, Util::eps );
	ASSERT_EQ( result.size(), 3 );
	EXPECT_NEAR( result[0], 0, Util::eps );
	EXPECT_NEAR( result[1], 0, Util::eps );
	EXPECT_NEAR( result[2], 2, Util::eps );
}
TEST( MixedIntegerLinearProgram, knapsack01_1 )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;

	double total_w = 3;
	std::vector<int> weight_list = { 1,1,1,1 };
	std::vector<int> profit_list = { 2,1,4,3 };

	const int n = 4;
	prob.lb.resize( n, 0 );
	prob.isMaximization = true;
	prob.objectivefunc = DenseRow( profit_list.begin(),profit_list.end() ).toSparseRow();
	prob.int_range.resize( n );
	for( int i = 0; i < n; i++ )
		prob.int_range[i] = { 0,1 };//01 knapscak problem
	for( int i = 0; i < n; i++ )
	{
		SparseRow sr;
		sr[i] = 1;
		prob.emplace_back( Equation<SparseRow>( sr, tRelation::kLE, prob.int_range[i].value().second ) );
	}
	//weight constraint
	prob.emplace_back( Equation<DenseRow>( DenseRow( weight_list.begin(), weight_list.end() ), tRelation::kLE, total_w ).toSparseEquation() );

	milp.SetMaxIteration( 100 );
	milp.SetTimelimit( 3600 );
	auto [r, ofv] = milp.SolveMILP( prob, result );

	EXPECT_EQ( r, MixedIntegerLinearProgramSolver::tError::kSuc );
	EXPECT_NEAR( ofv, 4 + 3 + 2, Util::eps );
	ASSERT_EQ( result.size(), 4 );
	EXPECT_NEAR( result[0], 1, Util::eps );
	EXPECT_NEAR( result[1], 0, Util::eps );
	EXPECT_NEAR( result[2], 1, Util::eps );
	EXPECT_NEAR( result[3], 1, Util::eps );
}
//data from https://en.wikipedia.org/wiki/Knapsack_problem
TEST( MixedIntegerLinearProgram, knapsack01_2 )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;

	double total_w = 67;
	std::vector<int> weight_list = { 23,26,20,18,32,27,29,26,30,27 };
	std::vector<int> profit_list = { 505,352,458,220,354,414,498,545,473,543 };

	const int n = 10;
	prob.lb.resize( n, 0 );
	prob.isMaximization = true;
	prob.objectivefunc = DenseRow( profit_list.begin(),profit_list.end() ).toSparseRow();
	prob.int_range.resize( n );
	for( int i = 0; i < n; i++ )
		prob.int_range[i] = { 0,1 };//01 knapscak problem
	for( int i = 0; i < n; i++ )
	{
		SparseRow sr;
		sr[i] = 1;
		prob.emplace_back( Equation<SparseRow>( sr, tRelation::kLE, prob.int_range[i].value().second ) );
	}
	//weight constraint
	prob.emplace_back( Equation<DenseRow>( DenseRow( weight_list.begin(), weight_list.end() ), tRelation::kLE, total_w ).toSparseEquation() );

	milp.SetMaxIteration( 1000 );
	milp.SetTimelimit( 3600 );
	auto [r, ofv] = milp.SolveMILP( prob, result );

	EXPECT_EQ( r, MixedIntegerLinearProgramSolver::tError::kSuc );
	EXPECT_NEAR( ofv, 1270, Util::eps );
	ASSERT_EQ( result.size(), 10 );
}
