#include "pch.h"
#include "MixedIntegerLinearProgram.h"

namespace Util::ORtool
{
MixedIntegerLinearProgramSolver::tError MixedIntegerLinearProgramSolver::do_iteration()
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
			assert( p != candidate.end() );
			top->AddChild( &*p );
		}
		if( ub <= range.second )
		{
			auto p = GenerateNewNode( *top, varidx, ub, tRelation::kGE, range.second );
			assert( p != candidate.end() );
			top->AddChild( &*p );
		}
	}
	top->isExpanded = true;

	return tError::kSuc;
}
}