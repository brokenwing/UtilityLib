#include "pch.h"
#include "SimulatedAnnealing.h"

using namespace Util::ORtool;

namespace
{
class SA_Sample :protected SimulatedAnnealing<double>
{
public:
	double init_x = 0;
	double max_step = 5;
	double prev = 0;

	double opt_result = 0;
	double last_pos = 0;

	bool run()
	{
		this->Config( 1, 100 );
		this->ConfigLog( 999 );
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
	bool test_log()const noexcept
	{
		return GetLogList().size() > 5;
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
	EXPECT_TRUE( sa.test_log() );
}

TEST( SimulatedAnnealing, DEMO_X_2 )
{
	SA_Sample sa;
	sa.init_x = 2;
	ASSERT_TRUE( sa.run() );
	EXPECT_TRUE( sa.test() );
	EXPECT_TRUE( sa.test_log() );
}

TEST( SimulatedAnnealing, DEMO_X_minus_2 )
{
	SA_Sample sa;
	sa.init_x = -2;
	ASSERT_TRUE( sa.run() );
	EXPECT_TRUE( sa.test() );
	EXPECT_TRUE( sa.test_log() );
}