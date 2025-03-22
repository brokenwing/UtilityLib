#pragma once
#include <vector>
#include <functional>

//  HungarianAlgorithm ��ʵ�����������㷨�����ڽ������ͼ�����ƥ�����⡣
//  ���㷨������ O(n^3) ��ʱ�����ҵ�һ������ͼ�����ƥ�䣬���� n ��ͼ�нڵ��������
//  �����ṩ���¹��ܣ�
//  - ��ʼ����ʹ�ø��������ҽڵ�����ʼ���㷨��
//  - ����ڽӱߣ����ڽӱ�����ӱߡ�
//  - ��⣺�������ͼ�����ƥ�䡣
//  - ��գ������㷨��״̬���Ա�������ڽ���µĶ���ͼƥ�����⡣
class HungarianAlgorithm
{
private:
	int numLeft = 0;  // ���ڵ���
	int numRight = 0; // �Ҳ�ڵ���
	std::vector<std::vector<int>> adjList; // ͼ���ڽӱ��ʾ
	std::vector<int> matchedRight; // �洢ÿ���Ҳ�ڵ�ƥ������ڵ�
	std::vector<bool> visited; // ��� DFS �з��ʹ����Ҳ�ڵ�
	int maxMatching = 0; // �洢���ƥ��Ĵ�С

public:
	HungarianAlgorithm()
	{}

	void Initialize( int leftCount, int rightCount )
	{
		numLeft = leftCount;
		numRight = rightCount;
		adjList.resize( leftCount + 1 ); // Ϊÿ�����ڵ�����ڽ��б�Ĵ�С
		matchedRight.resize( rightCount + 1, 0 );
		visited.resize( rightCount + 1, false );
		maxMatching = 0;
	}

	// ����ڽӱߵĺ���
	void AddAdjacency( int leftNode, int rightNode )
	{
		if( leftNode > 0 && leftNode <= numLeft && rightNode > 0 && rightNode <= numRight )
		{
			adjList[leftNode].push_back( rightNode );
		}
		//  ����ڵ�����Ч����ִ���κβ���
	}

	//  ����Ѱ������·�������������������
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
