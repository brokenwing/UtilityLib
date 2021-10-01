#include "pch.h"
#include "SimulatedAnnealing.h"

using namespace Util::ORtool;

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

TEST( SA_TSP_sample, sa_basic )
{
	SA_TSP_sample sa( 50 );
	sa.Config( 1, 10000, true, 50 );
	sa.ConfigLog( 10 );
	//sa.SetTmaxTmin( 0, 0.1, true );
	sa.Execute( 0 );

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
	SA_TSP_sample sa( 50 );
	sa.Config( 1, 10000, true, 50 );
	sa.ConfigLog( 10 );
	//sa.SetTmaxTmin( 0, 0.1, true );
	sa.ConfigResample( 10 );
	sa.Execute( 0 );

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