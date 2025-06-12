#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <functional>
#include <optional>

// ����RMQ�࣬ģ�����T��ʾ����Ԫ�ص�����
// Compare��ʾ�Ƚ������ͣ�Ĭ��Ϊstd::less<T>���ڲ�����Сֵ
template <typename T, typename Compare = std::less<T>>
class RMQ
{
private:
	// ϡ���ʹ��һάvector�洢��
	// st[i][j] ��ֵ���ڲ�ͨ�� st[i * (k_max + 1) + j] ����
	std::vector<T> st;
	std::vector<int> log_table;      // Ԥ����log2ֵ��log_table[i] = floor(log2(i))
	int n = 0;                           // ԭʼ����Ĵ�С
	int k_max = 0;                       // log2(n) �����ֵ�����ڼ���һά����
	Compare comp = Compare();                    // �Ƚ���ʵ��������ִ�бȽϲ���

	// Ԥ����log_table
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
	// ���캯��������һ������������Ϊԭʼ���飬����Ԥ����
	// ������Ԥ����׶δ洢�����ڲ���st��log_table��
	// ͬʱ��ʼ���Ƚ���ʵ��
	RMQ( const std::vector<T>& arr )
	{
		initialize( arr );
	}
	void initialize( const std::vector<T>& arr )
	{
		clear();
		n = (int)arr.size();
		if( n == 0 )
			return; // ���������账��

		precompute_log_table( n );
		k_max = log_table[n]; // ����log2(n)�����ֵ

		// ϡ�����ܴ�СΪ n * (k_max + 1)
		st.resize( n * ( k_max + 1 ) );

		// ϡ����ʼ������һ�� (����Ϊ2^0=1������)
		// st[i][0] ��Ӧ st[i * (k_max + 1) + 0]
		for( int i = 0; i < n; ++i )
			st[i * ( k_max + 1 ) + 0] = arr[i];

		// ��̬�滮���ϡ���
		// j��ʾ2^j�ĳ���
		for( int j = 1; j <= k_max; ++j )
		{
			// i��ʾ�������ʼ����
			// ע��i + (1 << j) ���ܳ���n
			for( int i = 0; i + ( 1 << j ) <= n; ++i )
			{
				// st[i][j] = comp(st[i][j-1], st[i + 2^(j-1)][j-1]) ? st[i][j-1] : st[i + 2^(j-1)][j-1]
				// ת��Ϊһά������
				// val1 = st[i][j-1] ��Ӧ st[i * (k_max + 1) + (j - 1)]
				// val2 = st[i + (1 << (j - 1))][j-1] ��Ӧ st[(i + (1 << (j - 1))) * (k_max + 1) + (j - 1)]
				T val1 = st[i * ( k_max + 1 ) + ( j - 1 )];
				T val2 = st[( i + ( 1 << ( j - 1 ) ) ) * ( k_max + 1 ) + ( j - 1 )];
				st[i * ( k_max + 1 ) + j] = comp( val1, val2 ) ? val1 : val2;
			}
		}
	}

	// ��ѯ�������� [L, R] ��ֵ (������)
	// L��R��0-based����
	std::optional<T> query( int L, int R ) const
	{
		// ��������Ƿ���Ч
		if( L < 0 || R >= n || L > R ) [[unlikely]]
			return std::nullopt;

		// �������䳤��
		int length = R - L + 1;
		// �ҵ�����k��ʹ��2^k <= length
		int k = log_table[length];

		// ���� [L, R] ���Ա���������Ϊ 2^k �������串��
		// ��һ���������� [L, L + 2^k - 1]����Ӧ st[L][k] -> st[L * (k_max + 1) + k]
		// �ڶ����������� [R - 2^k + 1, R]����Ӧ st[R - 2^k + 1][k] -> st[(R - (1 << k) + 1) * (k_max + 1) + k]
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