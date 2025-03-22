#pragma once
#include <vector>
#include <functional>

//  HungarianAlgorithm 类实现了匈牙利算法，用于解决二分图的最大匹配问题。
//  该算法可以在 O(n^3) 的时间内找到一个二分图的最大匹配，其中 n 是图中节点的数量。
//  该类提供以下功能：
//  - 初始化：使用给定的左右节点数初始化算法。
//  - 添加邻接边：向邻接表中添加边。
//  - 求解：计算二分图的最大匹配。
//  - 清空：重置算法的状态，以便可以用于解决新的二分图匹配问题。
class HungarianAlgorithm
{
private:
	int numLeft = 0;  // 左侧节点数
	int numRight = 0; // 右侧节点数
	std::vector<std::vector<int>> adjList; // 图的邻接表表示
	std::vector<int> matchedRight; // 存储每个右侧节点匹配的左侧节点
	std::vector<bool> visited; // 标记 DFS 中访问过的右侧节点
	int maxMatching = 0; // 存储最大匹配的大小

public:
	HungarianAlgorithm()
	{}

	void Initialize( int leftCount, int rightCount )
	{
		numLeft = leftCount;
		numRight = rightCount;
		adjList.resize( leftCount + 1 ); // 为每个左侧节点调整邻接列表的大小
		matchedRight.resize( rightCount + 1, 0 );
		visited.resize( rightCount + 1, false );
		maxMatching = 0;
	}

	// 添加邻接边的函数
	void AddAdjacency( int leftNode, int rightNode )
	{
		if( leftNode > 0 && leftNode <= numLeft && rightNode > 0 && rightNode <= numRight )
		{
			adjList[leftNode].push_back( rightNode );
		}
		//  如果节点编号无效，则不执行任何操作
	}

	//  用于寻找增广路径的深度优先搜索函数
	bool FindAugmentingPath( int leftNode )
	{
		for( int rightNode : adjList[leftNode] )
		{
			if( !visited[rightNode] )
			{
				visited[rightNode] = true;
				if( matchedRight[rightNode] == 0 || FindAugmentingPath( matchedRight[rightNode] ) )
				{
					matchedRight[rightNode] = leftNode;
					return true;
				}
			}
		}
		return false;
	}

	int Solve()
	{
		for( int i = 1; i <= numLeft; ++i )
		{
			std::fill( visited.begin(), visited.end(), false );
			if( FindAugmentingPath( i ) )
			{
				maxMatching++;
			}
		}
		return maxMatching;
	}

	void Clear()
	{
		numLeft = 0;
		numRight = 0;
		adjList.clear();
		matchedRight.clear();
		visited.clear();
		maxMatching = 0;
	}
};
