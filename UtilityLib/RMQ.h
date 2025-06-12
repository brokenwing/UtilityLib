#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>

// 定义RMQ类，模板参数T表示数组元素的类型
// Compare表示比较器类型，默认为std::less<T>用于查找最小值
template <typename T, typename Compare = std::less<T>>
class RMQ
{
private:
	// 稀疏表，使用一维vector存储。
	// st[i][j] 的值在内部通过 st[i * (k_max + 1) + j] 访问
	std::vector<T> st;
	std::vector<int> log_table;      // 预计算log2值，log_table[i] = floor(log2(i))
	int n = 0;                           // 原始数组的大小
	int k_max = 0;                       // log2(n) 的最大值，用于计算一维索引
	Compare comp = Compare();                    // 比较器实例，用于执行比较操作

	// 预处理log_table
	void precompute_log_table( int size )
	{
		log_table.resize( size + 1 );
		log_table[1] = 0; // log2(1) = 0
		for( int i = 2; i <= size; ++i )
		{
			log_table[i] = log_table[i >> 1] + 1;
		}
	}

public:
	// 构造函数：接受一个常量引用作为原始数组，进行预处理
	// 数据在预处理阶段存储在类内部的st和log_table中
	// 同时初始化比较器实例
	RMQ( const std::vector<T>& arr )
	{
		initialize( arr );
	}
	void initialize( const std::vector<T>& arr )
	{
		clear();
		n = (int)arr.size();
		if( n == 0 )
			return; // 空数组无需处理

		precompute_log_table( n );
		k_max = log_table[n]; // 计算log2(n)的最大值

		// 稀疏表的总大小为 n * (k_max + 1)
		st.resize( n * ( k_max + 1 ) );

		// 稀疏表初始化：第一列 (长度为2^0=1的区间)
		// st[i][0] 对应 st[i * (k_max + 1) + 0]
		for( int i = 0; i < n; ++i )
			st[i * ( k_max + 1 ) + 0] = arr[i];

		// 动态规划填充稀疏表
		// j表示2^j的长度
		for( int j = 1; j <= k_max; ++j )
		{
			// i表示区间的起始索引
			// 注意i + (1 << j) 不能超过n
			for( int i = 0; i + ( 1 << j ) <= n; ++i )
			{
				// st[i][j] = comp(st[i][j-1], st[i + 2^(j-1)][j-1]) ? st[i][j-1] : st[i + 2^(j-1)][j-1]
				// 转换为一维索引：
				// val1 = st[i][j-1] 对应 st[i * (k_max + 1) + (j - 1)]
				// val2 = st[i + (1 << (j - 1))][j-1] 对应 st[(i + (1 << (j - 1))) * (k_max + 1) + (j - 1)]
				T val1 = st[i * ( k_max + 1 ) + ( j - 1 )];
				T val2 = st[( i + ( 1 << ( j - 1 ) ) ) * ( k_max + 1 ) + ( j - 1 )];
				st[i * ( k_max + 1 ) + j] = comp( val1, val2 ) ? val1 : val2;
			}
		}
	}

	// 查询给定区间 [L, R] 的值 (闭区间)
	// L和R是0-based索引
	std::optional<T> query( int L, int R ) const
	{
		// 检查索引是否有效
		if( L < 0 || R >= n || L > R ) [[unlikely]]
			return std::nullopt;

		// 计算区间长度
		int length = R - L + 1;
		// 找到最大的k，使得2^k <= length
		int k = log_table[length];

		// 区间 [L, R] 可以被两个长度为 2^k 的子区间覆盖
		// 第一个子区间是 [L, L + 2^k - 1]，对应 st[L][k] -> st[L * (k_max + 1) + k]
		// 第二个子区间是 [R - 2^k + 1, R]，对应 st[R - 2^k + 1][k] -> st[(R - (1 << k) + 1) * (k_max + 1) + k]
		T val1 = st[L * ( k_max + 1 ) + k];
		T val2 = st[( R - ( 1 << k ) + 1 ) * ( k_max + 1 ) + k];

		return comp( val1, val2 ) ? val1 : val2;
	}

	void clear()
	{
		st.clear();
		log_table.clear();
		n = 0;
		k_max = 0;
	}
};