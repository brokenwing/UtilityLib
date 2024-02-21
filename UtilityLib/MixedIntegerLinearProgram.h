#pragma once
#include "SimplexMethod.h"

#include <vector>
#include <queue>
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
	struct SolutionStamp
	{
		double ofv = 0;
		double time_stamp = 0;
		std::int64_t lp_iteration_stamp = 0;
		int iteration_stamp = 0;
	};
	using SolutionStampList = std::deque<SolutionStamp>;

private:
	Timer time;
	double timelimit = 10;
	int maxiteration = INT_MAX;
	int iteration = 0;
	std::int64_t lp_iteration = 0;

	SolutionStampList solution_stamp_list;
	std::vector<double> best_result;
	double best_ofv = 0;

	struct TreeNode
	{
		static constexpr int MAX_CHILD = 2;

		NonStandardFormLinearProgram<SparseRow> data;
		SimplexMethod sm;
		std::vector<double> result;
		std::vector<std::optional<std::pair<int, int>>> range;//only for int
		double ofv = 0;
		SimplexMethod::tError status = SimplexMethod::tError::kInfeasible;

		int idx = -1;
		int cnt_child = 0;
		TreeNode* parent = nullptr;
		TreeNode* child[MAX_CHILD];
		double priority = 1;//big

		Equation<SparseRow> constraint;
		int split_var_idx = -1;
		int split_value = 0;
		tRelation condition = tRelation::kEQ;

		bool isExpanded = false;//inside heap or not
		bool isReleased = false;//as long as we have constraint, we can reconstruct the data

		TreeNode()
		{
			std::memset( child, 0, sizeof( child ) );
		}

		void AddChild( TreeNode* p )
		{
			if( p && cnt_child < MAX_CHILD ) [[likely]]
				child[cnt_child++] = p;
			else
				assert( 0 );
		}
		//clear unnecessary data
		void Release()
		{
			assert( !isExpanded );
			isReleased = true;
			data.Clear();
			sm.Clear();
			result.clear();
			range.clear();
		}
		Equation<SparseRow> GetConstraint()const
		{
			SparseRow sr;
			sr[split_var_idx] = 1;
			return Equation<SparseRow>( sr, condition, split_value );
		}
	};
	using TreeIter = std::list<TreeNode>::iterator;

	int cnt_node = 0;//index of candidate
	int n = 0;//var number
	TreeNode* root = nullptr;
	std::list<TreeNode> candidate;
	std::vector<TreeIter> heap;//expand priority

	//todo memory management

public:
	MixedIntegerLinearProgramSolver()
	{}
	const SolutionStampList& GetSolutionStampList()const noexcept	{		return solution_stamp_list;	}
	void SetTimelimit( double val )noexcept	{		timelimit = val;	}
	void SetMaxIteration( int val )noexcept	{		maxiteration = val;	}

	bool isTerminated()const
	{
		return iteration >= maxiteration || time.GetTime() >= timelimit;
	}

	//assume no INF solution
	template <row_type T>
	std::pair<tError, double> SolveMILP( const NonStandardFormMixedIntegerLinearProgram<T>& input, std::vector<double>& result )
	{
		if( input.empty() )
			return std::make_pair( tError::kFail, 0 );
		try
		{
			solution_stamp_list.clear();
			cnt_node = 0;
			time.SetTime();
			n = (int)input.lb.size();

			best_result.clear();
			best_ofv = 0;
			iteration = 0;
			lp_iteration = 0;

			candidate.clear();
			heap.clear();
			root = &candidate.emplace_back();

			//initial build
			root->idx = cnt_node++;
			root->range = input.int_range;
			root->data.isMaximization = input.isMaximization;
			root->data.lb = input.lb;
			root->data.objectivefunc = input.objectivefunc.toSparseRow();
			for( auto& e : input )
				root->data.emplace_back() = e.toSparseEquation();
			root->sm.SetPerturbation( true );
			root->sm.SetSeed( 0 );
			root->sm.SetTimelimit( timelimit );
			auto [r, ofv] = root->sm.SolveLP( root->data, root->result );
			root->ofv = ofv;
			root->status = r;
			if( root->sm.isTerminated() )
				return { tError::kInitialLPtimeout,0 };
			if( r != SimplexMethod::tError::kOptimum )
				return { tError::kFail,0 };//todo
			lp_iteration = root->sm.GetTotalIteration();

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
		if( !result.empty() )
		{
			for( int i = 0; i < n; i++ )
				if( input.int_range[i].has_value() )
					result[i] = std::round( result[i] );//round to int
			assert( input.CheckEquation( DenseRow( result.begin(), result.end() ).toSparseRow() ) );
		}

		if( best_result.empty() )
			return std::make_pair( tError::kFail, 0 );
		else
			return std::make_pair( tError::kSuc, best_ofv );
	}

	tError do_iteration();

	std::string Print()const
	{
		std::ostringstream ss;

		for( auto& e : candidate )
		{
			ss << PrintNode( e );
		}
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
	std::vector<Equation<SparseRow>> GetConstraintSequence( const TreeNode* p )const
	{
		std::vector<Equation<SparseRow>> ret;
		while( p && p != root )
		{
			ret.emplace_back( p->constraint );
			p = p->parent;
		}
		std::reverse( ret.begin(), ret.end() );
		return ret;
	}
	bool CalcPriority( TreeNode& p )
	{
		constexpr bool isTest = false;
#ifdef _DEBUG
		auto before_data = p.data;
#endif
		if constexpr( !isTest )
		{
			auto [r, val] = p.sm.SolveWithNewConstraints( p.data, p.result, p.constraint );
			p.priority = p.ofv = val;
			lp_iteration += p.sm.GetPhase2Iteration();
			return r == SimplexMethod::tError::kOptimum;
		}
		else//for debug
		{
			auto seq = GetConstraintSequence( &p );
			p.data = root->data;
			p.data.insert( p.data.end(), seq.begin(), seq.end() );
			auto [r1, val1] = p.sm.SolveLP( p.data, p.result );//std
			
			auto tmp = p;
			tmp.data = root->data;
			tmp.sm.SolveLP( tmp.data, tmp.result );
			auto [r2, val2] = tmp.sm.SolveWithNewConstraints( tmp.data, tmp.result, seq.begin(), seq.end() );

			if( r1 != r2 || Util::NEQ( val1, val2 ) )
				PrintNode( p );

			p = tmp;
			p.priority = p.ofv = val2;
			return r2 == SimplexMethod::tError::kOptimum;
		}
	}
	bool UpdateSolution( const TreeNode& p )
	{
		if( isWorseThanBest( p ) )
			return false;

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

		//update
		best_result = p.result;
		best_ofv = p.ofv;
		//add stamp
		auto& stamp = solution_stamp_list.emplace_back();
		stamp.ofv = best_ofv;
		stamp.iteration_stamp = iteration;
		stamp.time_stamp = time.GetSeconds();
		stamp.lp_iteration_stamp = lp_iteration;
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
		p.constraint = p.GetConstraint();

		const bool ok = CalcPriority( p );
		if( ok )
		{
			p.status = SimplexMethod::tError::kOptimum;
			heap.emplace_back( me );
			std::push_heap( heap.begin(), heap.end(), TreeNodeCompare );
		}
		else//no solution
		{
			me->isExpanded = true;
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