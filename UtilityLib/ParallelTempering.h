#pragma once
#include "SimulatedAnnealing.h"
#include "Util.h"

namespace Util::ORtool
{
class ParallelTempering
{
public:
	enum struct tMethod
	{
		kLinear,
		kExp,
	};

private:
	mutable std::mt19937 rng;
	std::uniform_real_distribution<double> rand01 = std::uniform_real_distribution<double>( 0.0, 1.0 );
	int max_interval = 100;//max iteration for each swap
	int min_interval = 10;
	int min_split = 1000;//default split

	tMethod m_method = tMethod::kLinear;
	static constexpr double RATIO_LIMIT = 10;//P=e^-det/(T*ratio)=(e^-det/T)^(1/ratio)=P0^(1/ratio) thus 0.01%^0.1 > 30% is enough
	static constexpr int STEP_CNT = 50;
	static constexpr double STEP_MAX = RATIO_LIMIT / STEP_CNT;
	static constexpr double STEP_MIN = 1.0 / STEP_CNT;

	double scale = 100;

public:
	ParallelTempering()
	{}
	void SetMethod( tMethod val )
	{
		m_method = val;
	}
	template <typename T>
	requires simulated_annealing_type<typename std::iterator_traits<T>::value_type>
	bool Execute( T begin, T end, unsigned int seed = 0 )
	{
		using sa_type = typename std::iterator_traits<T>::value_type;
		rng.seed( seed );

		RELEASE_VER_TRY;
		
		struct Node
		{
			T sol;
			double score = 0;
			double Tmax = 0;
			double Tmin = 0;
			double ratio = 1;//real T=Tmax*ratio, swap T <=> swap ratio
			double p_acc = 1;
			std::int64_t last_acc_cnt = 0;
			int ratioidx = -1;
			bool isInit = false;

			void swap( Node& other )
			{
				std::swap( ratio, other.ratio );
				std::swap( ratioidx, other.ratioidx );
			}
		};
		std::vector<Node> q;
		q.reserve( std::distance( begin, end ) );
		int rank = 0;
		for( auto it = begin; it != end; ++it )
		{
			q.emplace_back().sol = it;
			q.back().ratioidx = rank++;
		}
		if( q.empty() )
			return true;
		const int n = (int)q.size();
		const double timelimit = begin->GetTimeLimit();
		const std::int64_t max_iteration = begin->GetMaxIteration();

		for( auto &it : q )
		{
			it.sol->SimulatedAnnealing::Initialize();
			it.sol->DefaultHook( sa_type::tState::kInit );
		}

		//interval
		const int gap = (int)std::min( (std::int64_t)max_interval, std::max( (std::int64_t)min_interval, max_iteration / min_split ) );
		std::vector<double> dist;
		dist.reserve( n );
		if( n >= 2 )
			for( int k = 0; k < n; k++ )
				dist.emplace_back( 1.0 / ( n - 1 ) * k );

		int update_gap = 0;
		double swap_p = 1;

		int cnt_init = 0;
		double maxratio = 1;
		double minratio = 1;
		double avgT = 0;
		const auto assignRatio = [&]( const std::vector<double>& ratioList )->void
		{
			std::vector<int> idx2order;
			idx2order.reserve( n );
			for( int i = 0; i < n; i++ )
				idx2order.emplace_back( i );
			std::stable_sort( idx2order.begin(), idx2order.end(), [&q] ( int a, int b )->bool
			{
				return q[a].score < q[b].score;
			} );

			int k = 0;
			for( int idx : idx2order )
			{
				auto& cur = q[idx];
				cur.ratioidx = k;
				cur.ratio = ratioList[k++];
			}
		};

		std::vector<int> rank2idx;
		rank2idx.resize( n, -1 );

		bool stop = false;
		std::barrier guard( n, [&] ()noexcept
		{
			for( auto& it : q )
				it.score = it.sol->GetCurrScore();
			stop = false;
			for( auto& it : q )
				stop |= it.sol->Terminate();
			//Temperature initialization
			if( cnt_init != n )
			{
				for( auto& it : q )
					if( !it.isInit && it.sol->m_is_initialized_T )
					{
						++cnt_init;
						it.Tmax = it.sol->GetMaxTemperature();
						it.Tmin = it.sol->GetMinTemperature();
						it.isInit = true;
						avgT += it.Tmax;
					}
				if( cnt_init == n )
				{
					avgT /= n;
					assignRatio( BuildRatioList( 1 + STEP_MAX * STEP_CNT / 2, 1 - STEP_MIN * STEP_CNT / 2, dist ) );
				}
			}
			if( cnt_init != n || n <= 1 )
				return;

			//swap adj object
			int cnt_swap = 0;
			int tot = 0;
			std::uniform_int_distribution<int> randx( 0, n - 2 );
			std::fill( rank2idx.begin(), rank2idx.end(), -1 );
			for( int k = 0; k < n; k++ )
				rank2idx[q[k].ratioidx] = k;
			for( int k = 0; k < n * std::min( n, 2 ); k++ )
			{
				int i = randx( rng );
				int j = i + 1;
				i = rank2idx[i];
				j = rank2idx[j];

				const bool isSwap = SwapTest( avgT * q[i].ratio, avgT * q[j].ratio, q[i].score, q[j].score );
				if( isSwap )
				{
					q[i].swap( q[j] );
					rank2idx[q[i].ratioidx] = i;
					rank2idx[q[j].ratioidx] = j;
				}
				cnt_swap += isSwap;
				tot++;
			}
			swap_p = swap_p * 0.8 + 0.2 * ( cnt_swap * 1.0 / tot );

			//adjust dist by acc
			for( auto& e : q )
			{
				e.p_acc = e.p_acc * 0.5 + 0.5 * ( e.sol->GetAcceptCnt() - e.last_acc_cnt ) / gap;
				e.last_acc_cnt = e.sol->GetAcceptCnt();
			}
			for( int k = 1; k < n - 1; k++ )
			{
				const auto& cur = q[rank2idx[k]];
				const auto& prev = q[rank2idx[k - 1]];
				const auto& next = q[rank2idx[k + 1]];
				const double p = cur.p_acc;
				const double expected_p = ( prev.p_acc + next.p_acc ) * 0.5;//from some paper
				if( expected_p < 0.2 )
					continue;
				const double expected_std = 0.1;
				double norm_p = fabs( p - expected_p ) / ( expected_std * 2 );
				if( std::erf( norm_p ) > rand01( rng ) )
				{
					if( p > expected_p )
						dist[k] = dist[k - 1] + ( dist[k] - dist[k - 1] ) * 0.999;
					else
						dist[k] = dist[k + 1] - ( dist[k + 1] - dist[k] ) * 0.999;
				}
				//std::cout << int( cur.p_acc * 100 ) << ' ' << int(dist[k]*100) << ' ';
			}
			//std::cout << '\n';

			//update scale and ratio bound
			//if( update_gap++ >= 10 )
			{
				update_gap = 0;
				const double p = swap_p;
				const double expected_p = 0.01;//from some paper
				const double expected_std = expected_p;
				double norm_p = fabs( p - expected_p ) / ( expected_std * 2 );
				if( std::erf( norm_p ) > rand01( rng ) )
				{
					if( p > expected_p )
					{
						scale = std::min( scale * 1.1, 1000.0 );
						maxratio = std::max( 1.0, maxratio - STEP_MAX );
						minratio = std::max( minratio - STEP_MIN, 1.0 / RATIO_LIMIT );
					}
					else
					{
						maxratio = std::min( maxratio + STEP_MAX, RATIO_LIMIT );
						minratio = std::min( minratio + STEP_MIN, 1.0 );
						scale = std::max( scale * 0.9, 0.1 );
					}
				}
				//std::cout << p <<' '<<scale<<'\n';
			}
			assignRatio( BuildRatioList( maxratio, minratio, dist ) );
			//reset Temperature
			for( auto& it : q )
				it.sol->SetTmaxTmin( avgT * it.ratio, it.Tmin );
		} );
		auto task = [&] ( const int idx )->void
		{
			auto& cur = q[idx];
			while( !stop )
			{
				for( int i = 0; i < gap; i++ )
				{
					if( cur.sol->Terminate() )[[unlikely]]
						break;
					cur.sol->EvaluateStep();
					cur.sol->ResampleUpdate();
					cur.sol->UpdateStep();
				}
				guard.arrive_and_wait();
			}
		};

		std::vector<std::future<void>> thread_pool;
		thread_pool.reserve( n );
		for( int i = 0; i < n; i++ )
			thread_pool.emplace_back( std::async( task, i ) );

		for( auto& e : thread_pool )
			e.wait();

		for( auto &it : q )
			it.sol->DefaultHook( sa_type::tState::kFinish );

		RELEASE_VER_CATCH_START( const std::exception& );
		RELEASE_VER_CATCH_CONTENT( return false );
		RELEASE_VER_CATCH_START( ... );
		RELEASE_VER_CATCH_CONTENT( return false );
		RELEASE_VER_CATCH_END;
		return true;
	}

private:
	std::vector<double> BuildRatioList( double maxratio, double minratio, const std::vector<double>& offset )const
	{
		const int n = (int)offset.size();
		assert( n >= 2 );
		std::vector<double> ratioList;//ascend
		ratioList.reserve( n );

		maxratio = std::max( maxratio, 1 + eps );
		minratio = std::min( minratio, 1 - eps );
		minratio = std::max( minratio, 0 + eps );
		const double w_up = maxratio / ( 1 / minratio + maxratio );
		const double w_dn = 1 - w_up;
		
		switch( m_method )
		{
		case tMethod::kLinear:
			for( double r : offset )
			{
				if( r < w_up + eps )
				{
					double pos = r / w_up;
					double val = maxratio - ( maxratio - 1 ) * pos;
					ratioList.emplace_back( val );
				}
				else
				{
					double pos = ( r - w_up ) / w_dn;
					double val = 1 - ( 1 - minratio ) * pos;
					ratioList.emplace_back( val );
				}
			}
			break;
		case tMethod::kExp:
		{
			for( double r : offset )
			{
				if( r < w_up + eps )
				{
					double pos = r / w_up;
					double val = std::pow( maxratio, ( maxratio - 1 ) * pos );
					ratioList.emplace_back( val );
				}
				else
				{
					double pos = ( r - w_up ) / w_dn;
					double val = std::pow( minratio, ( 1 - minratio ) * pos );
					ratioList.emplace_back( val );
				}
			}
			break;
		}
		default:
			break;
		}
		std::sort( ratioList.begin(), ratioList.end() );
		return ratioList;
	}
	bool SwapTest( double T1, double T2, double score1, double score2 )const
	{
		std::uniform_real_distribution<double> rand01( 0.0, 1.0 );
		double val = ( 1 / T1 - 1 / T2 ) * ( score1 - score2 );
		bool isSwap = ( val > 0.01 ) ? true : ( std::exp( val ) > rand01( rng ) * scale );
		return isSwap;
	}
};
}