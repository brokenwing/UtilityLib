#pragma once
#include "pch.h"
#include "CommonDef.h"
#include "Timer.h"
#include "Util.h"

namespace Util::ORtool
{
template <typename Solution, typename Engine>
class SimulatedAnnealing;

//maxmize score (default)
template <typename Solution, typename Engine = Util::RNG>
class HillClimb
{
public:
	using SolutionType = Solution;
	using RNGType = Engine;

	enum struct tState
	{
		kInit = 1,
		kNeighbor = 2,
		kAcceptSolution = 4,
		kRollback = 8,
		kFinish = 16,
		kUndefinded = 32,
	};
	struct LogInfo
	{
		double score = 0;
		std::int64_t iteration = 0;
		double time_s = 0;
		double progress = 0;
		bool accept = false;
	};

	bool isMaximize = true;

private:
	Timer global_time;
	double m_timelimit_s = 1;//second
	std::int64_t m_max_iteration = 100;

	std::int64_t m_iteration = 0;

	Solution m_sol;
	double m_best_score = 0;
	double m_nxt_score = 0;
	
	LFA::deque<LogInfo> m_logList;
	bool m_is_collect = false;
	int m_collect_flag = (int)tState::kAcceptSolution | (int)tState::kRollback;

protected:
	mutable Engine rng;

public:
	friend class SimulatedAnnealing<Solution, Engine>;
	
	decltype( m_max_iteration ) GetMaxIteration()const noexcept	{		return m_max_iteration;	}
	double GetTimeLimit()const noexcept	{		return m_timelimit_s;	}

	void Config( double timelimit_s, std::int64_t max_iteration, bool collect_log = false, int collect_flag = (int)tState::kAcceptSolution | (int)tState::kRollback )noexcept
	{
		m_timelimit_s = timelimit_s;
		m_max_iteration = max_iteration;
		m_is_collect = collect_log;
		m_collect_flag = collect_flag;
	}
	const Solution& GetSolution()const noexcept	{		return m_sol;	}
	double GetScore()const noexcept	{		return m_best_score;	}
	decltype( rng )& GetRNG()const noexcept	{		return rng;	}
	const decltype( m_logList )& GetLogList()const noexcept	{		return m_logList;	}
	double GetElapsedTime()const	{		return global_time.GetTime();	}

	bool Execute( unsigned int seed = 0 );

protected:
	virtual void InitializeSolution( SolutionType& sol ) = 0;
	virtual void Neighbor( SolutionType& sol ) = 0;
	virtual void Rollback( SolutionType& sol ) = 0;
	virtual double CalcScore( const SolutionType& sol )const = 0;
	virtual void Hook( const tState state )	{}
	
	decltype( m_iteration )GetIteration()const noexcept	{		return m_iteration;	}
	double GetProgress()const	{		return std::max( global_time.GetTime() / m_timelimit_s, m_iteration / static_cast<double>( m_max_iteration ) );	}
	void DefaultHook( const tState state )
	{
		UpdateLog( state );
		Hook( state );
	}
	double GetNextScore()const noexcept	{		return m_nxt_score;	}
	bool Terminate()const	{		return global_time.GetTime() > m_timelimit_s || m_iteration >= m_max_iteration;	}
	void UpdateLog( const tState state );
};

template<typename Solution, typename Engine>
inline bool HillClimb<Solution, Engine>::Execute( unsigned int seed )
{
	RELEASE_VER_TRY;

	global_time.SetTime();
	rng = Engine( seed );
	m_iteration = 0;
	m_logList.clear();

	InitializeSolution( m_sol );
	m_best_score = CalcScore( m_sol );
	DefaultHook( tState::kInit );

	while( !Terminate() )
	{
		Neighbor( m_sol );
		m_nxt_score = CalcScore( m_sol );
		DefaultHook( tState::kNeighbor );
		if( isMaximize ? Util::GT( m_nxt_score, m_best_score ) : Util::LT( m_nxt_score, m_best_score ) )
		{
			m_best_score = m_nxt_score;
			DefaultHook( tState::kAcceptSolution );
		}
		else
		{
			Rollback( m_sol );
			DefaultHook( tState::kRollback );
		}
		++m_iteration;
	}

	RELEASE_VER_CATCH_START( const std::exception& );
	RELEASE_VER_CATCH_CONTENT( return false );
	RELEASE_VER_CATCH_START( ... );
	RELEASE_VER_CATCH_CONTENT( return false );
	RELEASE_VER_CATCH_END;

	return true;
}
template<typename Solution, typename Engine>
inline void HillClimb<Solution, Engine>::UpdateLog( const tState state )
{
	if( m_is_collect && ( m_collect_flag & (int)state ) != 0 )
	{
		auto& e = m_logList.emplace_back();
		e.iteration = GetIteration();
		e.time_s = global_time.GetTime();
		e.progress = GetProgress();
		e.accept = !( state == tState::kRollback );
		e.score = ( state == tState::kRollback || state == tState::kAcceptSolution ) ? GetNextScore() : GetScore();
	}
}

//Transform from SA to HC
template<typename SimulatedAnnealingEntity>
class HillClimbFromSimulatedAnnealing :public HillClimb<typename SimulatedAnnealingEntity::SolutionType, typename SimulatedAnnealingEntity::RNGType>
{
private:
	static_assert( std::is_base_of_v<SimulatedAnnealing<SimulatedAnnealingEntity::SolutionType,SimulatedAnnealingEntity::RNGType>, SimulatedAnnealingEntity>,
				   "SimulatedAnnealingEntity must inherit from SimulatedAnnealing" );

	SimulatedAnnealingEntity& me;
public:
	HillClimbFromSimulatedAnnealing( SimulatedAnnealingEntity& _me ) :me( _me )
	{
		this->isMaximize = _me.isMaximize;
	}

protected:
	void InitializeSolution( typename SimulatedAnnealingEntity::SolutionType& sol )override
	{
		me.InitializeSolution( sol );
	}
	void Neighbor( typename SimulatedAnnealingEntity::SolutionType& sol ) override
	{
		me.Neighbor( sol );
	}
	void Rollback( typename SimulatedAnnealingEntity::SolutionType& sol ) override
	{
		me.Rollback( sol );
	}
	double CalcScore( const typename SimulatedAnnealingEntity::SolutionType& sol )const override
	{
		return me.CalcScore( sol );
	}
};

}