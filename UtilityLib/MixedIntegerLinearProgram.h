#pragma once
#include "SimplexMethod.h"

#include <vector>
#include <deque>
#include <optional>
#include <assert.h>
#include <sstream>

namespace Util::ORtool
{
template <row_type T>
struct NonStandardFormMixedIntegerLinearProgram :public NonStandardFormLinearProgram<T>
{
	std::vector<std::optional<std::pair<int, int>>> int_range;

	constexpr bool isInteger( double val )const noexcept
	{
		return Util::IsZero( val - std::round( val ) );
	}
	template <typename Iter>
	bool CheckInteger( Iter st, Iter ed )const
	{
		return CheckInteger( T( st, ed ) );
	}
	bool CheckInteger( const T& result )const
	{
		const int n = (int)int_range.size();
		if constexpr( std::same_as<T, DenseRow> )
		{
			const int m = std::min( n, (int)result.size() );
			for( int i = 0; i < m; ++i )
				if( int_range[i].has_value() && !Util::IsZero( result[i] - std::round( result[i] ) ) )
					return false;
		}
		if constexpr( std::same_as<T, SparseRow> )
			for( auto [idx, val] : result )
				if( idx >= 0 && idx < n && int_range[idx].has_value() && !Util::IsZero( val - std::round( val ) ) )
					return false;
		return true;
	}
};



class MixedIntegerLinearProgramSolver
{
public:
	enum struct tError
	{
		kSuc,
		kInitialLPtimeout,//no enough time to finish first LP

		kException,
		kFail,
		kFinish,
	};
private:
	int maxiteration = INT_MAX;
	double timelimit = 10;
	Timer time;
	int iteration = 0;

	std::vector<double> best_result;
	double best_ofv = 0;

	struct TreeNode
	{
		NonStandardFormLinearProgram<SparseRow> data;
		SimplexMethod sm;
		std::vector<double> result;
		std::vector<std::optional<std::pair<int, int>>> range;//only for int
		double ofv = 0;
		SimplexMethod::tError status = SimplexMethod::tError::kInfeasible;

		int idx = -1;
		TreeNode* parent = nullptr;
		double priority = 1;//big

		int split_var_idx = -1;
		int split_value = 0;
		tRelation condition = tRelation::kEQ;
	};
	int cnt_node = 0;
	int n = 0;//var number
	std::list<TreeNode> candidate;
	using TreeIter = typename decltype( candidate )::iterator;
	std::vector<TreeIter> heap;

public:
	MixedIntegerLinearProgramSolver()
	{}
	void SetTimelimit( double val )noexcept
	{
		timelimit = val;
	}
	void SetMaxIteration( int val )noexcept
	{
		maxiteration = val;
	}

	bool isTerminated()const
	{
		return iteration >= maxiteration || time.GetTime() >= timelimit;
	}

	template <row_type T>
	std::pair<tError, double> SolveMILP( NonStandardFormMixedIntegerLinearProgram<T>& input, std::vector<double>& result )
	{
		if( input.empty() )
			return std::make_pair( tError::kFail, 0 );
		try
		{
			cnt_node = 0;
			time.SetTime();
			n = (int)input.lb.size();

			best_result.clear();
			best_ofv = 0;
			iteration = 0;

			candidate.clear();
			heap.clear();
			auto& root = candidate.emplace_back();

			//initial build
			root.idx = cnt_node++;
			root.range = input.int_range;
			root.data.lb = input.lb;
			root.data.objectivefunc = input.objectivefunc.toSparseRow();
			for( auto& e : input )
				root.data.emplace_back() = e.toSparseEquation();
			root.sm.SetPerturbation( true );
			root.sm.SetSeed( 0 );
			root.sm.SetTimelimit( timelimit );
			auto [r, ofv] = root.sm.SolveLP( input, root.result );
			root.ofv = ofv;
			root.status = r;
			if( root.sm.isTerminated() )
				return { tError::kInitialLPtimeout,0 };
			if( r != SimplexMethod::tError::kOptimum )
				return { tError::kFail,0 };//todo

			heap.emplace_back( candidate.begin() );

			//start
			while( !isTerminated() )
			{
				++iteration;

				do_iteration();
			}
		} catch( std::exception& )
		{
			return std::make_pair( tError::kException, 0 );
		}

		//std::cout << Print();

		result = best_result;
		if( best_result.empty() )
			return std::make_pair( tError::kFail, 0 );
		else
			return std::make_pair( tError::kSuc, best_ofv );
	}

	tError do_iteration()
	{
		if( heap.empty() )
			return tError::kFinish;

		auto top = heap.front();
		std::pop_heap( heap.begin(), heap.end(), TreeNodeCompare );
		heap.pop_back();

		if( isWorseThanBest( *top ) )
			return tError::kSuc;

		const int varidx = FindNextVarIdx( *top );
		if( varidx == -1 )
		{
			const bool ok = UpdateSolution( *top );
			assert( ok );
		}
		else
		{
			const double splitval = top->result[varidx];

			assert( top->range[varidx].has_value() );
			auto& range = top->range[varidx].value();
			assert( range.first <= range.second );

			int ub = static_cast<int>( Util::Ceil( top->result[varidx] ) );
			ub += Util::EQ( ub, splitval );
			assert( Util::GE( ub, top->result[varidx] ) );

			int lb = static_cast<int>( Util::Floor( top->result[varidx] ) );
			lb -= Util::EQ( lb, splitval );
			assert( Util::LE( lb, top->result[varidx] ) );

			if( lb >= range.first )
			{
				auto p = GenerateNewNode( *top, varidx, lb, tRelation::kLE, range.first );
			}
			if( ub <= range.second )
			{
				auto p = GenerateNewNode( *top, varidx, ub, tRelation::kGE, range.second );
			}
		}

		return tError::kSuc;
	}

	std::string Print()const
	{
		std::ostringstream ss;

		for( auto& e : candidate )
		{
			ss << PrintNode( e );
		}
		return ss.str();
	}
	std::string PrintNode( const TreeNode& p )const
	{
		std::ostringstream ss;
		const bool ok = p.status == SimplexMethod::tError::kOptimum;
		ss << "idx=" << p.idx;
		if( p.parent )
			ss << " from " << p.parent->idx;
		else
			ss << " root";
		ss << ( ok ? " OK" : " NG" );
		if( p.split_var_idx != -1 )
		{
			ss << "  ";
			ss << '[' << p.split_var_idx << ']' << ( p.condition == tRelation::kLE ? "<=" : ">=" ) << p.split_value << '\n';
		}
		else
			ss << '\n';
		return ss.str();
	}

private:
	static bool isInt( double val )
	{
		return Util::IsZero( val - std::round( val ) );
	}
	static bool TreeNodeCompare( TreeIter a, TreeIter b )
	{
		return Util::GT( a->priority, b->priority );
	}
	//if LP result worse than best, skip
	bool isWorseThanBest( const TreeNode& p )const
	{
		if( !best_result.empty() )
		{
			if( p.data.isMaximization )
				return Util::GE( best_ofv, p.ofv );
			else
				return Util::LE( best_ofv, p.ofv );
		}
		return false;
	}

	bool CalcPriority( TreeNode& p )
	{
#ifdef _DEBUG
		auto before_data = p.data;
#endif
		SparseRow sr;
		sr[p.split_var_idx] = 1;
		Equation<SparseRow> new_constraint( sr, p.condition, p.split_value );
		//if( p.idx == 29 )
			//puts( "" );
		auto [r, val] = p.sm.SolveWithNewConstraints( p.data, p.result, new_constraint );
		p.priority = val;
		p.ofv = val;

		return r == SimplexMethod::tError::kOptimum;
	}
	bool UpdateSolution( const TreeNode& p )
	{
		//check
		bool flag = true;
		for( int i = 0; i < n; i++ )
		{
			if( !p.range[i].has_value() )
				continue;
			auto range = p.range[i].value();
			const double val = p.result[i];
			if( Util::LT( val, range.first ) || Util::GT( val, range.second ) || !isInt( val ) )
			{
				flag = false;
				break;
			}
		}
		if( !flag )
			return false;

		if( best_result.empty() || ( p.data.isMaximization ? Util::LT( best_ofv, p.ofv ) : Util::GT( best_ofv, p.ofv ) ) )
		{
			best_result = p.result;
			best_ofv = p.ofv;
		}
		return true;
	}
	TreeIter GenerateNewNode( const TreeNode& old, const int varidx, const int splitval, tRelation rel, int other )
	{
		assert( varidx != -1 );
		auto& p = candidate.emplace_back();
		TreeIter me = std::prev( candidate.end() );

		p.data = old.data;
		p.idx = cnt_node++;
		p.parent = const_cast<TreeNode*>( &old );
		p.sm = old.sm;
		p.sm.SetPerturbation( true );
		p.sm.SetSeed( 0 );
		p.sm.SetTimelimit( timelimit );
		p.range = old.range;

		p.split_var_idx = varidx;
		p.split_value = splitval;
		p.condition = rel;
		if( rel == tRelation::kLE )
			p.range[varidx] = std::make_pair( other, splitval );
		else
			p.range[varidx] = std::make_pair( splitval, other );
		assert( old.range[varidx].has_value() );
		assert( p.range[varidx].value().first >= old.range[varidx].value().first );
		assert( p.range[varidx].value().second <= old.range[varidx].value().second );
		assert( p.range[varidx].value() != old.range[varidx].value() );

		const bool ok = CalcPriority( p );
		if( ok )
		{
			p.status = SimplexMethod::tError::kOptimum;
			heap.emplace_back( me );
			std::push_heap( heap.begin(), heap.end(), TreeNodeCompare );
		}
		return me;
	}
	int FindNextVarIdx( const TreeNode& p )const
	{
		double best_w = 0;
		int ret = -1;

		for( int i = 0; i < n; i++ )
		{
			if( !p.range[i].has_value() )
				continue;
			if( isInt( p.result[i] ) )
				continue;

			auto range = p.range[i].value();
			assert( range.first <= range.second );
			if( range.first == range.second )
				continue;

			const double w = std::fabs( p.data.objectivefunc[i] );

			if( ret == -1 || Util::LT( best_w, w ) )
			{
				ret = i;
				best_w = w;
			}
		}

		return ret;
	}
};
}