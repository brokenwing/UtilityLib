#include "pch.h"
#include "SimulatedAnnealing.h"
#include "HillClimb.h"

using namespace Util::ORtool;

namespace
{
class HC_Sample :public HillClimb<std::pair<int, int>>
{
private:
	SolutionType backup;
	
	static constexpr int N = 4;
	int MAP[4][4] = { {1,2,3,4},
					  {12,13,14,5},
					  {11,16,15,6},
					  {10,9,8,7} };

public:
	SolutionType init_pos;

public:
	HC_Sample()
	{}
	void InitializeSolution( SolutionType& sol )override
	{
		sol = init_pos;
	}
	void Neighbor( SolutionType& sol ) override
	{
		backup = sol;
		constexpr int N_DIR = 4;
		int dx[N] = { 1,-1,0,0 };
		int dy[N] = { 0,0,1,-1 };
		int w[N] = { 0 };
		FOR( i, 0, N_DIR )
		{
			auto tmp = sol;
			tmp.first += dx[i];
			tmp.second += dy[i];
			if( tmp.first >= 0 && tmp.second >= 0 && tmp.first < N && tmp.second < N )
			{
				w[i] = 1;
			}
		}
		std::discrete_distribution<int> r( w, w + N );
		const int idx = r( GetRNG() );
		sol.first += dx[idx];
		sol.second += dy[idx];
	}
	void Rollback( SolutionType& sol )override
	{
		sol = backup;
	}
	double CalcScore( const SolutionType& sol )const override
	{
		return MAP[sol.first][sol.second];
	}
};
}

TEST( HC_Sample, opt_at_first )
{
	HC_Sample hc;
	hc.init_pos = { 2,1 };
	hc.Config( 1, 10 );
	ASSERT_TRUE( hc.Execute( 0 ) );
	EXPECT_DOUBLE_EQ( hc.GetScore(), 16 );
	EXPECT_EQ( hc.GetSolution(), std::make_pair( 2, 1 ) );
	EXPECT_TRUE( hc.GetLogList().empty() );
}
TEST( HC_Sample, minimize_test )
{
	HC_Sample hc;
	hc.isMaximize = false;
	hc.init_pos = { 2,1 };
	hc.Config( 1, 100 );
	ASSERT_TRUE( hc.Execute( 0 ) );
	EXPECT_DOUBLE_EQ( hc.GetScore(), 1 );
	EXPECT_EQ( hc.GetSolution(), std::make_pair( 0, 0 ) );
}
TEST( HC_Sample, opt_at_first_log )
{
	HC_Sample hc;
	hc.init_pos = { 2,1 };
	hc.Config( 1, 10, true, (int)HC_Sample::tState::kAcceptSolution );
	ASSERT_TRUE( hc.Execute( 0 ) );
	EXPECT_TRUE( hc.GetLogList().empty() );
}
TEST( HC_Sample, worst_to_opt )
{
	HC_Sample hc;
	hc.init_pos = { 0,0 };
	hc.Config( 1, 100, true );
	ASSERT_TRUE( hc.Execute( 0 ) );
	EXPECT_DOUBLE_EQ( hc.GetScore(), 16 );
	EXPECT_EQ( hc.GetSolution(), std::make_pair( 2, 1 ) );
	EXPECT_TRUE( hc.GetLogList().size() > 3 );
	auto q = hc.GetLogList();
	double prev = 0;
	for( auto& e : q )
		if( e.accept )
		{
			EXPECT_GT( e.score, prev );
			prev = e.score;
			std::cout << e.score << ' ';
		}
	std::cout << '\n';
}
TEST( HC_Sample, twice_run )
{
	HC_Sample hc;
	hc.init_pos = { 0,0 };
	hc.Config( 1, 100, true );
	ASSERT_TRUE( hc.Execute( 0 ) );
	EXPECT_DOUBLE_EQ( hc.GetScore(), 16 );
	EXPECT_EQ( hc.GetSolution(), std::make_pair( 2, 1 ) );
	EXPECT_TRUE( hc.GetLogList().size() > 3 );

	ASSERT_TRUE( hc.Execute( 0 ) );
	EXPECT_DOUBLE_EQ( hc.GetScore(), 16 );
	EXPECT_EQ( hc.GetSolution(), std::make_pair( 2, 1 ) );
	EXPECT_TRUE( hc.GetLogList().size() > 3 );
}

namespace
{
class SA_Sample :public SimulatedAnnealing<double>
{
public:
	double init_x = 0;
	double max_step = 5;
	double prev = 0;

	double opt_result = 0;
	double last_pos = 0;

	int log_flag = 0;

	bool run()
	{
		this->Config( 1, 100, true, 5 );
		this->ConfigLog( 999, log_flag );
		this->SetTmaxTmin( 100, 0.01 );
		this->SetTemperatureCalcType( tCoolDownType::kExponential );
		bool r = this->Execute( 0 );
		opt_result = GetSolution();
		last_pos = GetCurrSolution();
		return r;
	}

	//local min test
	bool test()const noexcept
	{
		return ( last_pos < -1.6 && last_pos > -1.7 ) || ( last_pos > 0.85 && last_pos < 1 );
	}

	// Inherited via SimulatedAnnealing
	void InitializeSolution( double& sol ) override
	{
		prev = sol = init_x;
	}
	void Neighbor( double& sol ) override
	{
		std::uniform_int_distribution randlr( 0, 1 );
		prev = sol;
		bool lr = randlr( GetRNG() );
		double step = std::max( 0.01, ( 1 - GetProgress() ) * ( 1 - GetProgress() ) * max_step );
		sol += step * ( lr ? 1 : -1 );
	}
	void Rollback( double& sol ) override
	{
		sol = prev;
	}
	double CalcScore( const double& x ) const override
	{
		//x^4+x^3-3x^2+1
		//local min at (-1.66,-4.2) / (0.91,0.05)
		//local max at (0,1)
		double val = x * x * x * x + x * x * x - 3 * x * x + 1;
		return -val;
	}
};
}

TEST( SimulatedAnnealing, DEMO_X_0 )
{
	SA_Sample sa;
	sa.init_x = 0;
	ASSERT_TRUE( sa.run() );
	EXPECT_TRUE( sa.test() );
}

TEST( SimulatedAnnealing, DEMO_X_2 )
{
	SA_Sample sa;
	sa.init_x = 2;
	ASSERT_TRUE( sa.run() );
	EXPECT_TRUE( sa.test() );
}

TEST( SimulatedAnnealing, DEMO_X_minus_2 )
{
	SA_Sample sa;
	sa.init_x = -2;
	ASSERT_TRUE( sa.run() );
	EXPECT_TRUE( sa.test() );
}

TEST( SimulatedAnnealing, Log_1 )
{
	using State = Util::ORtool::SimulatedAnnealing<double>::tState;
	SA_Sample sa;
	sa.init_x = 0;
	sa.log_flag = (int)State::kAcceptSolution | (int)State::kFinish;
	ASSERT_TRUE( sa.run() );
	EXPECT_TRUE( sa.GetLogList().size() > 5 );
	int n = (int)sa.GetLogList().size();
	FOR( i, 1, n )
	{
		EXPECT_TRUE( sa.GetLogList()[i - 1].iteration <= sa.GetLogList()[i].iteration );
		EXPECT_TRUE( sa.GetLogList()[i - 1].progress <= sa.GetLogList()[i].progress );
		EXPECT_TRUE( sa.GetLogList()[i - 1].time_s <= sa.GetLogList()[i].time_s );
	}
}

TEST( SimulatedAnnealing, Log_2 )
{
	using State = Util::ORtool::SimulatedAnnealing<double>::tState;
	SA_Sample sa;
	sa.init_x = 0;
	sa.log_flag = (int)State::kFinish;
	ASSERT_TRUE( sa.run() );
	EXPECT_TRUE( sa.GetLogList().size() == 1 );
}

TEST( SimulatedAnnealing, Log_3 )
{
	using State = Util::ORtool::SimulatedAnnealing<double>::tState;
	SA_Sample sa;
	sa.init_x = 0;
	sa.log_flag = (int)State::kUpdateSolution;
	ASSERT_TRUE( sa.run() );
	int n = (int)sa.GetLogList().size();
	FOR( i, 1, n )
		EXPECT_TRUE( sa.GetLogList()[i - 1].score <= sa.GetLogList()[i].score );
}

TEST( SimulatedAnnealing, resampleT )
{
	using State = Util::ORtool::SimulatedAnnealing<double>::tState;
	SA_Sample sa;
	sa.init_x = 0;
	sa.log_flag = (int)State::kResampleT;
	sa.ConfigResample( 10 );
	ASSERT_TRUE( sa.run() );
	int n = (int)sa.GetLogList().size();
	EXPECT_GT( n, 3 );
}

namespace
{
struct TSPnode :public std::vector<int>
{
};
class SA_TSP_sample :public Util::ORtool::SimulatedAnnealing<TSPnode>
{
	int n = 0;
	std::vector<std::vector<int>> dis;
	TSPnode backup;
	Util::RNG my_rng;

public:
	SA_TSP_sample( int _n, int seed = 0, int w_min = 1, int w_max = 1000 )
	{
		n = _n;
		my_rng = Util::RNG( seed );
		std::uniform_int_distribution<int> randD( w_min, w_max );
		dis.resize( n );
		for( auto& e : dis )
			e.resize( n );

		FOR( i, 0, n )
			FOR( j, 0, n )
			{
				if( i == j )
					dis[i][j] = 0;
				else
					dis[i][j] = randD( my_rng );
			}
	}

	// Inherited via SimulatedAnnealing
	virtual void InitializeSolution( TSPnode& sol ) override
	{
		sol.resize( n );
		FOR( i, 0, n )
			sol[i] = i;
	}
	virtual void Neighbor( TSPnode& sol ) override
	{
		backup = sol;
		//swap
		std::uniform_int_distribution<int> randx( 0, n - 1 );
		const int a = randx( my_rng );
		const int b = randx( my_rng );
		std::swap( sol[a], sol[b] );
	}
	virtual void Rollback( TSPnode& sol ) override
	{
		sol = backup;
	}
	virtual double CalcScore( const TSPnode& sol ) const override
	{
		int tot = 0;
		FOR( i, 0, n )
			tot += dis[sol[i]][sol[( i + 1 ) % n]];
		return -tot;
	}
};
}

TEST( SA_TSP_sample, minimize_test )
{
	int n = 10;
	SA_TSP_sample sa( n );
	sa.isMaximize = false;
	sa.Config( 1, 100, true, n );
	sa.ConfigLog( 10 );
	sa.Execute( 0 );
	
	auto q = sa.GetLogList();
	EXPECT_GT( q.size(), 5 );
	double prev = q.front().best_score;
	for( auto& e : q )
	{
		EXPECT_LE( e.best_score, prev );
		prev = e.best_score;
	}
}
TEST( SA_TSP_sample, sa_basic )
{
	int n = 50;
	SA_TSP_sample sa( n );
	sa.Config( 1, 10000, true, n );
	sa.ConfigLog( 10 );
	//sa.SetTmaxTmin( 0, 0.1, true );
	ASSERT_TRUE( sa.Execute( 0 ) );

	auto q = sa.GetLogList();
	EXPECT_GT( q.size(), 5 );
	std::cout << "OFV= ";
	for( auto& e : q )
		std::cout << e.best_score << ' ';
	std::cout << '\n';
	std::cout << "T= ";
	for( auto& e : q )
		std::cout << e.temperature << ' ';
	std::cout << '\n';
}
TEST( SA_TSP_sample, sa_basic_resample )
{
	int n = 50;
	SA_TSP_sample sa( n );
	sa.Config( 1, 10000, true, n );
	sa.ConfigLog( 10 );
	//sa.SetTmaxTmin( 0, 0.1, true );
	sa.ConfigResample( 10 );
	ASSERT_TRUE( sa.Execute( 0 ) );

	auto q = sa.GetLogList();
	EXPECT_GT( q.size(), 5 );
	std::cout << "OFV= ";
	for( auto& e : q )
		std::cout << e.best_score << ' ';
	std::cout << '\n';
	std::cout << "T= ";
	for( auto& e : q )
		std::cout << e.temperature << ' ';
	std::cout << '\n';
}