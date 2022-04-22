#pragma once
#include "pch.h"
#include "CommonDef.h"
#include "Timer.h"
#include "Util.h"
#include <barrier>
#include <future>

namespace Util::ORtool
{
template <typename Solution, typename Engine>
class HillClimb;
template <typename Solution, typename Engine>
class SimulatedAnnealing;
template <typename T>
concept simulated_annealing_type = std::is_base_of_v<SimulatedAnnealing<typename T::SolutionType, typename T::RNGType>, T>;

//maxmize score
template <typename Solution, typename Engine = Util::RNG>
class SimulatedAnnealing
{
public:
	using SolutionType = Solution;
	using RNGType = Engine;

	enum struct tCoolDownType
	{
		kLinear,
		kExponential,
	};
	enum struct tState
	{
		kInit = 1,
		kNeighbor = 2,
		kUpdateSolution = 4,
		kAcceptSolution = 8,
		kRollBack = 16,
		kFinish = 32,
		kResampleT = 64,
		kUndefinded = 128,
	};
	struct LogInfo
	{
		double score = 0;
		double best_score = 0;
		std::int64_t iteration = 0;
		double time_s = 0;
		double progress = 0;
		double temperature = 0;
	};
	bool isMaximize = true;

private:
	Timer global_time;
	std::int64_t m_iteration = 0;
	double m_cur_T = 0;
	double m_progress = 0;//0-1

	Solution opt_solution;
	double m_opt_score = 0;
	Solution m_current;
	double m_score = 0;
	double m_nxt_score = 0;

	double T_max = 0;
	double T_min = 1e-3;
	bool m_auto_estimate_T = true;

	double m_timelimit_s = 1;//second
	std::int64_t m_max_iteration = 100;
	bool m_progress_calc_from_iteration = true;//Calc Progress by time or iteration
	size_t m_sample_size = 300;//size of sample for estimating Tmax
	tCoolDownType m_temperature_calc_type = tCoolDownType::kExponential;
	
	std::deque<LogInfo> m_logList;
	std::int64_t m_max_log_size = 0;
	std::int64_t m_iteration_per_log = 0;
	int m_collect_flag = (int)tState::kAcceptSolution | (int)tState::kFinish;
	
	typedef std::deque<std::pair<double, double>> SampleListType;
	bool m_is_initialized_T = false;
	double m_p0 = 0.9;
	int m_cnt_recalcT = 0;
	std::int64_t m_iteration_for_resample = 0;
	std::int64_t m_last_sample_iteration = 0;
	SampleListType m_resampleList;
protected:
	mutable Engine rng;

public:
	friend class HillClimb<Solution, Engine>;

	decltype( m_max_iteration ) GetMaxIteration()const noexcept	{		return m_max_iteration;	}
	double GetTimeLimit()const noexcept	{		return m_timelimit_s;	}

	//Record information after each state (or finish) iff iteration changes >= m_max_iteration/max_log_size
	void ConfigLog( std::int64_t max_log_size = 0, int flag = (int)tState::kAcceptSolution | (int)tState::kFinish ) noexcept;
	//ReCalc max/min Temperature during SA for at most n times
	void ConfigResample( int n, double p0 = 0.9 )noexcept	{		m_cnt_recalcT = n; m_p0 = p0;	}
	void Config( double timelimit_s, std::int64_t max_iteration, bool progress_calc_from_iteration = true, size_t sample_size = 300 )noexcept;
	void SetTmaxTmin( double t_max, double t_min, bool auto_estimate = false )noexcept;
	void SetTemperatureCalcType( const tCoolDownType val )noexcept	{		m_temperature_calc_type = val;	}

	std::pair<double, double> GetInitialTemperature()const noexcept	{		return std::make_pair( T_min, T_max );	}
	const Solution& GetSolution()const noexcept	{		return opt_solution;	}
	double GetScore()const noexcept	{		return m_opt_score;	}
	decltype( rng )& GetRNG()const noexcept	{		return rng;	}
	const decltype( m_logList )& GetLogList()const noexcept	{		return m_logList;	}
	double GetElapsedTime()const	{		return global_time.GetTime();	}
	decltype( m_iteration )GetIteration()const noexcept	{		return m_iteration;	}

	bool Execute( unsigned int seed = 0 );
	bool ParallelExecute( const int n_thread, unsigned int seed = 0 );
	void EvaluateStep()
	{
		//calc T
		std::tie( m_progress, m_cur_T ) = CalcTemperature( m_temperature_calc_type );
		Neighbor( m_current );
		m_nxt_score = CalcScore( m_current );
		DefaultHook( tState::kNeighbor );
	}
	void ResampleUpdate();
	void UpdateStep();
	void UpdateStep( const SolutionType& nxt_sol )
	{
		if( Accept( m_score, m_nxt_score, m_cur_T ) )
		{
			m_current = nxt_sol;
			//update opt
			if( isBetter( m_nxt_score, m_opt_score ) )
			{
				opt_solution = nxt_sol;
				m_opt_score = m_nxt_score;
				DefaultHook( tState::kUpdateSolution );
			}
			DefaultHook( tState::kAcceptSolution );
			m_score = m_nxt_score;
		}
		else
		{
			DefaultHook( tState::kRollBack );
		}
	}

protected:
	virtual void InitializeSolution( SolutionType& sol ) = 0;
	virtual void Neighbor( SolutionType& sol ) = 0;
	virtual void Rollback( SolutionType& sol ) = 0;
	virtual double CalcScore( const SolutionType& sol )const = 0;
	virtual void Hook( const tState state )	{}
	virtual void ParallelNeighbor( int thread_idx, SolutionType& sol, RNGType& rng )
	{
		Neighbor( sol );
	}
	virtual double ParallelCalcScore( int thread_idx, const SolutionType& sol )const
	{
		return CalcScore( sol );
	}
	
	double GetProgress()const noexcept	{		return m_progress;	}
	const Solution& GetCurrSolution()const noexcept	{		return m_current;	}
	double GetCurrScore()const noexcept	{		return m_score;	}
	double GetCurrTemperature()const noexcept	{		return m_cur_T;	}

	void DefaultHook( const tState state )
	{
		UpdateLog( state );
		Hook( state );
	}
	void UpdateLog( const tState state );
	double GetNextScore()const noexcept	{		return m_nxt_score;	}
	bool isBetter( const double nxt_score, const double old_score )const noexcept	{		return isMaximize ? GT( nxt_score, old_score ) : LT( nxt_score, old_score );	}

	void Initialize();
	bool Accept( const double old_score, const double new_score, const double temperature )const;
	bool Terminate()const	{		return global_time.GetTime() > m_timelimit_s || m_iteration >= m_max_iteration;	}
	
	//Reference:https://se.mathworks.com/matlabcentral/answers/uploaded_files/14677/B:COAP.0000044187.23143.bd.pdf
	//<Score_min,Score_max>, p0 is accept prob, it says that p=1 is ok for most case
	//calc the max/min with only worser solution
	static std::optional<double> EstimateTemperature( const SampleListType& sample, const double p0, const int p = 1, const int MAX_ITERATION = 1000 );

private:
	//n_try until find worse solution
	SampleListType GenerateSample( Solution start, const size_t sample_size, const int n_try = 3 );

	//<progress ratio,T>
	std::tuple<double, double> CalcTemperature( const tCoolDownType type )const;
};

//Record information after each state (or finish) iff iteration changes >= m_max_iteration/max_log_size
template<typename Solution, typename Engine>
inline void SimulatedAnnealing<Solution, Engine>::ConfigLog( std::int64_t max_log_size, int flag ) noexcept
{
	m_max_log_size = max_log_size;
	m_collect_flag = flag;
	m_iteration_per_log = 0;
	if( m_max_log_size != 0 )
	{
		m_iteration_per_log = m_max_iteration / m_max_log_size;
		m_iteration_per_log += m_iteration_per_log == 0;
	}
}

template<typename Solution, typename Engine>
inline void SimulatedAnnealing<Solution, Engine>::Config( double timelimit_s, std::int64_t max_iteration, bool progress_calc_from_iteration, size_t sample_size ) noexcept
{
	m_timelimit_s = timelimit_s;
	m_max_iteration = max_iteration;
	m_progress_calc_from_iteration = progress_calc_from_iteration;
	m_sample_size = sample_size;
}

template<typename Solution, typename Engine>
inline void SimulatedAnnealing<Solution, Engine>::SetTmaxTmin( double t_max, double t_min, bool auto_estimate ) noexcept
{
	T_max = t_max;
	T_min = t_min;
	m_auto_estimate_T = auto_estimate;
}

template<typename Solution, typename Engine>
inline bool SimulatedAnnealing<Solution, Engine>::Execute( unsigned int seed )
{
	using namespace Util;
	RELEASE_VER_TRY;

	rng = Engine( seed );
	Initialize();
	DefaultHook( tState::kInit );

	while( !Terminate() )
	{
		EvaluateStep();
		ResampleUpdate();
		UpdateStep();
	}

	DefaultHook( tState::kFinish );

	RELEASE_VER_CATCH_START( const std::exception& );
	RELEASE_VER_CATCH_CONTENT( return false );
	RELEASE_VER_CATCH_START( ... );
	RELEASE_VER_CATCH_CONTENT( return false );
	RELEASE_VER_CATCH_END;

	return true;
}

//ParallelNeighbor(...) and ParallelCalcScore(...) must be parallel executable
//tState::kNeighbor won't update cursolution (todo)
//no rollback
template<typename Solution, typename Engine>
inline bool SimulatedAnnealing<Solution, Engine>::ParallelExecute( const int n_thread, unsigned int seed )
{
	if( n_thread < 1 )
		return false;

	rng = RNGType( seed );
	struct TempSolution
	{
		int idx = -1;
		double score = 0;
		RNGType rng;
		Solution sol;
	};
	std::vector<TempSolution> solQ;
	solQ.resize( n_thread );
	for( int idx = 0; auto & e : solQ )
	{
		e.idx = idx++;
		e.rng = RNGType( rng() );
	}
	Initialize();
	DefaultHook( tState::kInit );

	bool stop = Terminate();
	std::barrier guard( n_thread, [&] ()noexcept
	{
		auto best = std::min_element( solQ.begin(), solQ.end(), [this] ( auto& a, auto& b )
		{
			return this->isBetter( a.score, b.score );
		} );

		std::tie( m_progress, m_cur_T ) = CalcTemperature( m_temperature_calc_type );
		//m_current = sol;
		m_nxt_score = best->score;
		DefaultHook( tState::kNeighbor );

		ResampleUpdate();
		UpdateStep( best->sol );

		++m_iteration;

		stop = Terminate();
	} );
	auto task = [&] ( const int idx )->void
	{
		TempSolution& e = solQ[idx];
		while( !stop )
		{
			e.score = 0;
			e.sol = this->GetCurrSolution();
			ParallelNeighbor( e.idx, e.sol, e.rng );
			e.score = ParallelCalcScore( e.idx, e.sol );
			guard.arrive_and_wait();
		}
	};
	
	std::vector<std::future<void>> thread_pool;
	thread_pool.reserve( n_thread );
	for( int i = 0; i < n_thread; i++ )
		thread_pool.emplace_back( std::async( task, i ) );

	for( auto& e : thread_pool )
		e.wait();

	DefaultHook( tState::kFinish );

	return true;
}

template<typename Solution, typename Engine>
inline void SimulatedAnnealing<Solution, Engine>::ResampleUpdate()
{
	if( ( m_cnt_recalcT > 0 || !m_is_initialized_T ) && isBetter( m_score, m_nxt_score ) )
	{
		m_resampleList.emplace_back( std::min( m_nxt_score, m_score ), std::max( m_nxt_score, m_score ) );
		if( m_resampleList.size() > m_sample_size )
			m_resampleList.pop_front();
		if( ( m_resampleList.size() >= m_sample_size || ( !m_is_initialized_T && GetProgress() > 0.1 ) )
			&& ( !m_is_initialized_T || m_iteration_for_resample + m_last_sample_iteration <= m_iteration ) )
		{
			double p0 = m_p0 * ( 1 - GetProgress() );
			auto Tnxt = EstimateTemperature( m_resampleList, std::max( p0, 0.1 ) );
			double t = T_max;
			if( Tnxt.has_value() && Tnxt.value() > 3 )
				t = Tnxt.value();
			this->SetTmaxTmin( t, T_min );
			m_last_sample_iteration = m_iteration;
			m_is_initialized_T = true;
			DefaultHook( tState::kResampleT );
		}
	}
}

template<typename Solution, typename Engine>
inline void SimulatedAnnealing<Solution, Engine>::UpdateStep()
{
	if( Accept( m_score, m_nxt_score, m_cur_T ) )
	{
		//update opt
		if( isBetter( m_nxt_score, m_opt_score ) )
		{
			opt_solution = m_current;
			m_opt_score = m_nxt_score;
			DefaultHook( tState::kUpdateSolution );
		}
		DefaultHook( tState::kAcceptSolution );
		m_score = m_nxt_score;
	}
	else
	{
		//roll back
		Rollback( m_current );
		DefaultHook( tState::kRollBack );
	}

	++m_iteration;
}

template<typename Solution, typename Engine>
inline void SimulatedAnnealing<Solution, Engine>::UpdateLog( const tState state )
{
	if( m_max_log_size > 0 && ( (int)state & m_collect_flag ) != 0
		&& ( state == tState::kFinish || m_logList.empty() || m_logList.back().iteration + m_iteration_per_log <= m_iteration ) )
	{
		m_logList.emplace_back( GetCurrScore(), GetScore(), GetIteration(), global_time.GetTime(), GetProgress(), GetCurrTemperature() );
	}
}

template<typename Solution, typename Engine>
void SimulatedAnnealing<Solution, Engine>::Initialize()
{
	using namespace Util;
	global_time.SetTime();
	m_logList.clear();
	m_last_sample_iteration = 0;
	m_resampleList.clear();
	m_iteration = 0;
	m_is_initialized_T = !m_auto_estimate_T;
	if( m_cnt_recalcT > 0 )
		m_iteration_for_resample = std::max( 1LL, m_max_iteration / m_cnt_recalcT );

	if( m_auto_estimate_T )
	{
		InitializeSolution( m_current );
		T_max = 1e60;
	}

	InitializeSolution( m_current );
	m_opt_score = m_score = CalcScore( m_current );
	opt_solution = m_current;
}

template<typename Solution, typename Engine>
bool SimulatedAnnealing<Solution, Engine>::Accept( const double old_score, const double new_score, const double temperature ) const
{
	if( !m_is_initialized_T )
		return true;
	using namespace Util;
	static const std::uniform_real_distribution<double> rand_real_01 = std::uniform_real_distribution<double>( 0.0, 1.0 );

	double diff = new_score - old_score;
	assert( !isnan( diff ) );
	if( !isMaximize )
		diff = -diff;
	if( GT( diff, 0 ) )
		return true;

	assert( GE( temperature, 0 ) );
	diff *= 1.0 / ( temperature + 1e-8 );
	assert( !isnan( std::exp( diff ) ) );
	if( LE( rand_real_01( rng ), std::exp( diff ) ) )
		return true;
	return false;
}

//Reference:https://se.mathworks.com/matlabcentral/answers/uploaded_files/14677/B:COAP.0000044187.23143.bd.pdf
//<Score_min,Score_max>, p0 is accept prob, it says that p=1 is ok for most case
//calc the max/min with only worser solution
template<typename Solution, typename Engine>
std::optional<double> SimulatedAnnealing<Solution, Engine>::EstimateTemperature( const SampleListType& sample, const double p0, const int p, const int MAX_ITERATION )
{
	using namespace Util;
	if( sample.empty() )
		return 1;//maybe score does not change, no need to try high T_max
	if( LE( p0, 0 ) || GE( p0, 1 ) )
		return std::nullopt;

	double T1 = 0;//any positive number is ok, but the following is recommended (to prevent overflow)
	for( auto & e : sample )
		T1 += std::fabs( e.second - e.first );
	T1 /= sample.size();
	T1 /= -std::log( p0 );
	double Tdefault = T1;
	if( LE( T1, 0 ) || std::isnan( T1 ) )
		return std::nullopt;

	double p1;//current prob
	int n = 0;//loop counter
	do
	{
		n++;
		double s1 = 0, s2 = 0;
		for( auto & e : sample )
		{
			s1 += std::exp( -e.second / T1 );//MAX
			s2 += std::exp( -e.first / T1 );//MIN
		}
		p1 = s1 / s2;
		T1 *= std::pow( std::log( p1 ) / std::log( p0 ), 1.0 / p );
		//error check
		if( std::isnan( T1 ) || n > MAX_ITERATION )
		{
//try p=2 since p=1 might not converge
			if( p != 2 )
				return EstimateTemperature( sample, p0, 2, MAX_ITERATION );
			return Tdefault;
		}
	} while( std::fabs( p0 - p1 ) > 1e-4 );//check if converge
	return T1;
}

//n_try until find worse solution
template<typename Solution, typename Engine>
inline SimulatedAnnealing<Solution, Engine>::SampleListType SimulatedAnnealing<Solution, Engine>::GenerateSample( Solution start, const size_t sample_size, const int n_try )
{
	using namespace Util;

	//generate sample
	SampleListType sample;
	for( int i = 0; i < (int)sample_size; i++ )
	{
		double cur = CalcScore( start );
		for( int j = 0; j < n_try; j++ )
		{
			Neighbor( start );
			double nxt = CalcScore( start );
			//only consider worse value
			if( isBetter( cur, nxt ) )
			{
				sample.emplace_back( nxt, cur );
				break;
			}
			Rollback( start );
		}
		Neighbor( start );
	}
	return sample;
}

//<progress ratio,T>
template<typename Solution, typename Engine>
inline std::tuple<double, double> SimulatedAnnealing<Solution, Engine>::CalcTemperature( const tCoolDownType type ) const
{
	double complete_rate;
	if( m_progress_calc_from_iteration )
		complete_rate = m_iteration / static_cast<double>( m_max_iteration );
	else
		complete_rate = global_time.GetTime() / m_timelimit_s;
	double decayRate = T_min / T_max;

	double val = 1;
	switch( type )
	{
	case tCoolDownType::kLinear:
		//Linear
		val = T_max * ( 1 - complete_rate );
		break;
	case tCoolDownType::kExponential:
		//Exponential
		val = T_max * std::pow( decayRate, complete_rate );
		break;
	default:
		val = 1e20;
		break;
	}
	return { complete_rate,val };
}
}