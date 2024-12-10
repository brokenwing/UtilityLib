#include "pch.h"
#include "Tarjan.h"

const std::vector<int>& Util::Tarjan::solve( const tMethod method )
{
	const int n = (int)st.size();
	dfn.clear();
	dfn.resize( n, -1 );
	low.clear();
	low.resize( n, -1 );
	mark.clear();
	mark.resize( n, tState::kNew );
	while( !stk.empty() )
		stk.pop();
	index = 0;

	group.clear();
	for( int i = 0; i < n; i++ )
		group.emplace_back( i );
	sum.clear();
	for( int i = 0; i < n; i++ )
		sum.emplace_back( 1 );

	for( int i = 0; i < n; i++ )
		if( mark[i] == tState::kNew )
		{
			switch( method )
			{
			case tMethod::kDFS:
				dfs( i );
				break;
			case tMethod::kNonDFS:
				nondfs( i );
				break;

			case tMethod::kAuto:
			default:
				if( n <= MAX_DFS_SIZE )
					dfs( i );
				else
					nondfs( i );
				break;
			}
		}

	return group;
}

void Util::Tarjan::dfs( const int p )
{
	dfn[p] = low[p] = ++index;
	stk.push( p );
	mark[p] = tState::kUsing;

	for( int i = st[p]; i != -1; i = edgeList[i].next )
	{
		if( mark[edgeList[i].pt] == tState::kNew )
		{
			dfs( edgeList[i].pt );
			low[p] = std::min( low[p], low[edgeList[i].pt] );
		}
		else if( mark[edgeList[i].pt] == tState::kUsing )
			low[p] = std::min( low[p], dfn[edgeList[i].pt] );
	}
	if( dfn[p] == low[p] )
	{
		int i = stk.top();
		while( i != p )
		{
			mark[i] = tState::kFinish;
			group[i] = p;
			sum[p] += sum[i];
			sum[i] = 0;
			stk.pop();
			i = stk.top();
		}
		mark[p] = tState::kFinish;
		stk.pop();
	}
}

//simulate dfs

void Util::Tarjan::nondfs( const int p )
{
	struct StackInfo
	{
		int next = -1;
		int pt = -1;
		int step = 0;
	};
	std::stack<StackInfo> temp;
	temp.push( StackInfo( st[p], p, 0 ) );

	while( !temp.empty() )
	{
		auto& head = temp.top();
		if( head.step == 0 )
		{
			dfn[head.pt] = low[head.pt] = ++index;
			stk.push( head.pt );
			mark[head.pt] = tState::kUsing;
			head.step = 1;
		}
		if( head.step == -1 )
		{
			low[head.pt] = std::min( low[head.pt], low[edgeList[head.next].pt] );
			head.step = 1;
		}
		if( head.step == 1 )
		{
			bool nextround = false;
			while( head.next != -1 )
			{
				if( mark[edgeList[head.next].pt] == tState::kNew )
				{
					int x = edgeList[head.next].pt;
					head.step = -1;
					temp.push( StackInfo( st[x], x, 0 ) );//do not use head after this
					nextround = true;
					break;
				}
				else if( mark[edgeList[head.next].pt] == tState::kUsing )
					low[head.pt] = std::min( low[head.pt], dfn[edgeList[head.next].pt] );
				head.next = edgeList[head.next].next;
			}
			if( nextround )
				continue;
			if( head.next == -1 )
				head.step = 2;
		}
		if( head.step == 2 )
		{
			const int cur_pt = head.pt;
			if( dfn[cur_pt] == low[cur_pt] )
			{
				int i = stk.top();
				while( i != cur_pt )
				{
					mark[i] = tState::kFinish;
					group[i] = cur_pt;
					sum[cur_pt] += sum[i];
					stk.pop();
					i = stk.top();
				}
				mark[cur_pt] = tState::kFinish;
				stk.pop();
			}
			temp.pop();
		}
	}
}
