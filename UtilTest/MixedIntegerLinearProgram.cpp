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
	ASSERT_EQ( result.size(), n );
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
	ASSERT_EQ( result.size(), n );
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
	ASSERT_EQ( result.size(), n );
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
	ASSERT_EQ( result.size(), n );
}
TEST( MixedIntegerLinearProgram, multiknapsack_1 )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;

	double total_w = 29;
	std::vector<int> weight_list = { 5,10,12 };
	std::vector<int> profit_list = { 1,2,3 };

	const int n = 3;
	prob.lb.resize( n, 0 );
	prob.isMaximization = true;
	prob.objectivefunc = DenseRow( profit_list.begin(),profit_list.end() ).toSparseRow();
	prob.int_range.resize( n );
	for( int i = 0; i < n; i++ )
		prob.int_range[i] = { 0,2 };
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
	EXPECT_NEAR( ofv, 7, Util::eps );
	ASSERT_EQ( result.size(), n );
	EXPECT_NEAR( result[0], 1, Util::eps );
	EXPECT_NEAR( result[1], 0, Util::eps );
	EXPECT_NEAR( result[2], 2, Util::eps );
}
//data from https://developers.google.com/optimization/assignment/assignment_example?hl=zh-cn#c++_1
TEST( MixedIntegerLinearProgram, gap_1 )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;

	const int n_res = 5;
	const int m_job = 4;
	const int cost[n_res][m_job] = {
	{90, 80, 75, 70},   {35, 85, 55, 65},   {125, 95, 90, 95},
	{45, 110, 95, 115}, {50, 100, 90, 100},
	};

	const auto get_idx = [] ( int x, int y )->int
	{
		return x * m_job + y;
	};

	const int n = n_res * m_job;//<i,j> means res i do job j

	std::vector<double> cost_list;
	cost_list.resize( n, 0 );
	for( int i = 0; i < n_res; i++ )
		for( int j = 0; j < m_job; j++ )
			cost_list[get_idx( i, j )] = cost[i][j];

	prob.lb.resize( n, 0 );
	prob.isMaximization = false;
	prob.objectivefunc = DenseRow( cost_list.begin(),cost_list.end() ).toSparseRow();
	prob.int_range.resize( n );
	for( int i = 0; i < n; i++ )
		prob.int_range[i] = { 0,1 };
	for( int i = 0; i < n; i++ )
	{
		SparseRow sr;
		sr[i] = 1;
		prob.emplace_back( Equation<SparseRow>( sr, tRelation::kLE, prob.int_range[i].value().second ) );
	}
	//each res has at most 1 job
	for( int i = 0; i < n_res; i++ )
	{
		std::vector<double> tmp;
		tmp.resize( n, 0 );
		for( int j = 0; j < m_job; j++ )
			tmp[get_idx( i, j )] = 1;
		prob.emplace_back( Equation<DenseRow>( DenseRow( tmp.begin(), tmp.end() ), tRelation::kLE, 1 ).toSparseEquation() );
	}
	//each job must be done
	for( int j = 0; j < m_job; j++ )
	{
		std::vector<double> tmp;
		tmp.resize( n, 0 );
		for( int i = 0; i < n_res; i++ )
			tmp[get_idx( i, j )] = 1;
		prob.emplace_back( Equation<DenseRow>( DenseRow( tmp.begin(), tmp.end() ), tRelation::kEQ, 1 ).toSparseEquation() );
	}

	milp.SetMaxIteration( 100 );
	milp.SetTimelimit( 3600 );
	auto [r, ofv] = milp.SolveMILP( prob, result );

	EXPECT_EQ( r, MixedIntegerLinearProgramSolver::tError::kSuc );
	EXPECT_NEAR( ofv, 265, Util::eps );
	ASSERT_EQ( result.size(), n );
	int res2job[n_res];
	for( int i = 0; i < n_res; i++ )
	{
		res2job[i] = -1;
		for( int j = 0; j < m_job; j++ )
		{
			int x = static_cast<int>( std::round( result[get_idx( i, j )] ) );
			if( x == 1 )
				res2job[i] = j;
		}
	}
	EXPECT_EQ( res2job[0], 3 );
	EXPECT_EQ( res2job[1], 2 );
	EXPECT_EQ( res2job[2], 1 );
	EXPECT_EQ( res2job[3], 0 );
	EXPECT_EQ( res2job[4], -1 );
}
