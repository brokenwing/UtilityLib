#pragma once
#include "pch.h"
#include <stack>

namespace Util
{
//find strongly connected component
class Tarjan
{
public:
	static constexpr int MAX_DFS_SIZE = 10000;//max possible size of graph using DFS
	enum struct tMethod
	{
		kAuto,
		kDFS,
		kNonDFS,
	};

private:
	struct Node
	{
		int pt = -1;
		int next = -1;
	};
	std::vector<int> st;
	std::vector<Node> edgeList;

	//temp
	enum struct tState
	{
		kNew,
		kUsing,
		kFinish,
	};
	std::vector<int> dfn;
	std::vector<int> low;
	std::vector<tState> mark;
	std::stack<int> stk;
	int index = -1;

	std::vector<int> group;
	std::vector<int> sum;

public:
	const std::vector<int>& get_result_group()const			{		return group;	}
	const std::vector<int>& get_result_group_size()const	{		return sum;		}
	void add( int from_p1, int to_p2 )
	{
		auto& e = edgeList.emplace_back();
		e.pt = to_p2;
		e.next = st[from_p1];
		st[from_p1] = static_cast<int>( edgeList.size() ) - 1;
	}
	void reset( int size_n )
	{
		clear();
		st.resize( size_n, -1 );
	}
	void clear()
	{
		st.clear();
		edgeList.clear();
	}

	const std::vector<int>& solve( const tMethod method = tMethod::kAuto );

private:
	void dfs( const int p );
	//simulate dfs
	void nondfs( const int p );
};

}