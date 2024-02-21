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
//data https://developers.google.com/optimization/mip/mip_example?hl=zh-cn
TEST( MixedIntegerLinearProgram, easy3 )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;

	const int n = 2;
	prob.lb.resize( n, 0 );
	prob.isMaximization = true;
	prob.objectivefunc = DenseRow( {1,10} ).toSparseRow();
	prob.int_range.resize( n );
	prob.int_range[0] = { 0,3 };
	prob.int_range[1] = { 0,100 };
	for( int i = 0; i < n; i++ )
	{
		SparseRow sr;
		sr[i] = 1;
		prob.emplace_back( Equation<SparseRow>( sr, tRelation::kLE, prob.int_range[i].value().second ) );
	}
	prob.emplace_back( Equation<DenseRow>( { 1,7 }, tRelation::kLE, 17.5 ).toSparseEquation() );

	milp.SetMaxIteration( 100 );
	milp.SetTimelimit( 3600 );
	auto [r, ofv] = milp.SolveMILP( prob, result );

	EXPECT_EQ( r, MixedIntegerLinearProgramSolver::tError::kSuc );
	EXPECT_NEAR( ofv, 23, Util::eps );
	ASSERT_EQ( result.size(), n );
	EXPECT_NEAR( result[0], 3, Util::eps );
	EXPECT_NEAR( result[1], 2, Util::eps );
}
//more than 1 solution, data from https://developers.google.com/optimization/mip/mip_var_array?hl=zh-cn
TEST( MixedIntegerLinearProgram, normal1 )
{
	MixedIntegerLinearProgramSolver milp;
	NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
	std::vector<double> result;

	const int n = 5;
	prob.lb.resize( n, 0 );
	prob.isMaximization = true;
	prob.objectivefunc = DenseRow( {7,8,2,9,6} ).toSparseRow();
	prob.int_range.resize( n );
	for( int i = 0; i < n; i++ )
		prob.int_range[i] = { 0,100 };
	for( int i = 0; i < n; i++ )
	{
		SparseRow sr;
		sr[i] = 1;
		prob.emplace_back( Equation<SparseRow>( sr, tRelation::kLE, prob.int_range[i].value().second ) );
	}
	prob.emplace_back( Equation<DenseRow>( { 5,7,9,2,1 }, tRelation::kLE, 250 ).toSparseEquation() );
	prob.emplace_back( Equation<DenseRow>( { 18,4,-9,10,12 }, tRelation::kLE, 285 ).toSparseEquation() );
	prob.emplace_back( Equation<DenseRow>( { 4,7,3,8,5 }, tRelation::kLE, 211 ).toSparseEquation() );
	prob.emplace_back( Equation<DenseRow>( { 5,13,16,3,-7 }, tRelation::kLE, 315 ).toSparseEquation() );

	milp.SetMaxIteration( 500 );
	milp.SetTimelimit( 3600 );
	auto [r, ofv] = milp.SolveMILP( prob, result );

	auto sr = DenseRow( result.begin(), result.end() ).toSparseRow();
	EXPECT_EQ( r, MixedIntegerLinearProgramSolver::tError::kSuc );
	EXPECT_TRUE( prob.CheckEquation( sr ) );
	EXPECT_NEAR( ofv, 260, Util::eps );
	ASSERT_EQ( result.size(), n );
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

//check by enumerating solution (all int)
TEST( MixedIntegerLinearProgram, randomtest )
{
#ifdef _DEBUG
	return;
#endif
	const int n_test = 1000;
	const int max_enumeration = 1000;
	const int var_upperbound = 10;
	const int maxval = 20;
	const int maxrhs = 100;
	bool valid_first = false;
	std::mt19937 rng( 0 );
	std::uniform_int_distribution<int> randupperbound( 1, var_upperbound );
	std::uniform_int_distribution<int> randm( 1, 20 );
	std::uniform_int_distribution<int> randmaxmin( 0, 1 );
	std::uniform_int_distribution<int> randint( ( valid_first ? 0 : -maxval ), maxval );
	std::uniform_int_distribution<int> randrhs( ( valid_first ? 0 : -maxrhs ), maxrhs );
	const auto randomDenseRow = [&]( const int n )->DenseRow
	{
		DenseRow dr;
		dr.reserve( n );
		for( int i = 0; i < n; i++ )
			dr.emplace_back( randint( rng ) );
		return dr;
	};
	const auto randRelation = [&rng,valid_first] ()->tRelation
	{
		std::uniform_int_distribution<int> randx( ( valid_first ? 3 : 1 ), 3 );
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

	for( int i = 0; i < n_test; i++ )
	{
		int n = 0;
		const int m = randm( rng );

		std::vector<int> upperbound;
		int product = max_enumeration;
		while( product > 1 )
		{
			int x = randupperbound( rng );
			x = std::min( product - 1, x );
			assert( x > 0 );
			upperbound.emplace_back( x );
			product /= x + 1;
		}
		n = (int)upperbound.size();
		assert( n > 0 );

		MixedIntegerLinearProgramSolver milp;
		NonStandardFormMixedIntegerLinearProgram<SparseRow> prob;
		std::vector<double> result;

		prob.lb.resize( n, 0 );
		prob.isMaximization = randmaxmin( rng );
		prob.objectivefunc = randomDenseRow( n ).toSparseRow();
		prob.int_range.resize( n );
		for( int j = 0; j < n; j++ )
			prob.int_range[j] = { 0,upperbound[j] };
		for( int j = 0; j < n; j++ )
		{
			SparseRow sr;
			sr[j] = 1;
			prob.emplace_back( Equation<SparseRow>( sr, tRelation::kLE, prob.int_range[j].value().second ) );
		}
		for( int j = 0; j < m; j++ )
			prob.emplace_back( randomEquation( n ).toSparseEquation() );

		//solve by milp
		milp.SetMaxIteration( max_enumeration / 10 );
		milp.SetTimelimit( 3 );
		auto [r, ofv] = milp.SolveMILP( prob, result );

		//solve by brute
		std::vector<int> cur, best_sol;
		cur.resize( n, 0 );
		double best_val = 0;
		while( true )
		{
			DenseRow dr;
			dr.assign( cur.begin(), cur.end() );
			auto sr = dr.toSparseRow();
			bool ok = prob.CheckEquation( sr );
			if( ok )
			{
				double val = prob.objectivefunc.dot( dr );
				if( best_sol.empty() || ( prob.isMaximization ? Util::GT( val, best_val ) : Util::LT( val, best_val ) ) )
				{
					best_sol = cur;
					best_val = val;
				}
			}

			if( 0 )
			{
				std::cout << ok << ':';
				for( int x : cur )
					std::cout << ' ' << x;
				std::cout << '\n';
			}

			//next
			bool flag = true;
			for( int j = 0; j < n; j++ )
			{
				cur[j] = ( cur[j] + 1 ) % ( upperbound[j] + 1 );
				if( cur[j] != 0 )
					break;
				if( cur[j] == 0 && j == n - 1 )
					flag = false;
			}
			if( !flag )
				break;
		}

		if( best_sol.empty() )
		{
			ASSERT_EQ( r, MixedIntegerLinearProgramSolver::tError::kFail );
			continue;
		}
		else
			ASSERT_EQ( r, MixedIntegerLinearProgramSolver::tError::kSuc );
		
		ASSERT_NEAR( best_val, ofv, Util::eps );
	}
}