#include "pch.h"
#include "RMQ.h"

TEST( RMQ, empty )
{
    RMQ<int> x;
}
TEST( RMQ, set_compare )
{
	struct CMP
	{
		bool flag = false;
		bool operator()( const int a, const int b ) const
		{
            return ( a < b ) ^ flag;
		}
	};
    {
        RMQ<int, CMP> rmq( CMP( true ) );
        std::vector<int> data = { 1, 2, 3, 4, 5 };
        rmq.initialize( data );
        ASSERT_TRUE( rmq.query( 0, 4 ).has_value() );
        EXPECT_EQ( rmq.query( 0, 4 ).value(), 5 ); // �����������ֵ
    }
    {
        RMQ<int, CMP> rmq( CMP( false ) );
        std::vector<int> data = { 1, 2, 3, 4, 5 };
        rmq.initialize( data );
        ASSERT_TRUE( rmq.query( 0, 4 ).has_value() );
        EXPECT_EQ( rmq.query( 0, 4 ).value(), 1 );
    }
}

// --- ��Сֵ��ѯ���� (Ĭ�ϱȽ��� std::less<T>) ---

// ���Թ��캯���Ϳ��������
TEST(RMQ, HandlesEmptyArray) {
    std::vector<int> empty_int_data = {};
    RMQ<int> rmq_empty_int(empty_int_data);
    ASSERT_FALSE(rmq_empty_int.query(0, 0).has_value());

    std::vector<std::string> empty_string_data = {};
    RMQ<std::string> rmq_empty_string(empty_string_data);
    ASSERT_FALSE(rmq_empty_string.query(0, 0).has_value());
}

// ���Ե�Ԫ������
TEST(RMQ, HandlesSingleElementArray) {
    std::vector<int> single_int_data = {42};
    RMQ<int> rmq_single_int(single_int_data);
    ASSERT_TRUE(rmq_single_int.query(0, 0).has_value());
    EXPECT_EQ(rmq_single_int.query(0, 0).value(), 42);
    ASSERT_FALSE(rmq_single_int.query(0, 1).has_value()); // Խ��
    ASSERT_FALSE(rmq_single_int.query(-1, 0).has_value()); // Խ��

    std::vector<std::string> single_string_data = {"apple"};
    RMQ<std::string> rmq_single_string(single_string_data);
    ASSERT_TRUE(rmq_single_string.query(0, 0).has_value());
    EXPECT_EQ(rmq_single_string.query(0, 0).value(), "apple");
    ASSERT_FALSE(rmq_single_string.query(0, 1).has_value()); // Խ��
    ASSERT_FALSE(rmq_single_string.query(-1, 0).has_value()); // Խ��
}

// ���������������Сֵ��ѯ
TEST(RMQ, IntegerMinQuery) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data); // Ĭ�� std::less<int>

    // ���Ͳ�ѯ
    ASSERT_TRUE(rmq.query(0, 9).has_value());
    EXPECT_EQ(rmq.query(0, 9).value(), 0); // ����������Сֵ

    ASSERT_TRUE(rmq.query(1, 5).has_value());
    EXPECT_EQ(rmq.query(1, 5).value(), 2); // {3, 2, 5, 4, 6} -> 2

    ASSERT_TRUE(rmq.query(6, 6).has_value());
    EXPECT_EQ(rmq.query(6, 6).value(), 0); // ����Ԫ�� {0} -> 0

    ASSERT_TRUE(rmq.query(0, 0).has_value());
    EXPECT_EQ(rmq.query(0, 0).value(), 1); // ����Ԫ�� {1} -> 1

    ASSERT_TRUE(rmq.query(7, 8).has_value());
    EXPECT_EQ(rmq.query(7, 8).value(), 7); // {9, 7} -> 7
}

// ���������Դ���ߵ�һ��Ԫ�ؿ�ʼ����Сֵ��ѯ��Χ
TEST(RMQ, MinQueryStartsFromZero) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data);

    ASSERT_TRUE(rmq.query(0, 2).has_value());
    EXPECT_EQ(rmq.query(0, 2).value(), 1); // {1, 3, 2} -> 1

    ASSERT_TRUE(rmq.query(0, 3).has_value());
    EXPECT_EQ(rmq.query(0, 3).value(), 1); // {1, 3, 2, 5} -> 1

    ASSERT_TRUE(rmq.query(0, 6).has_value());
    EXPECT_EQ(rmq.query(0, 6).value(), 0); // {1, 3, 2, 5, 4, 6, 0} -> 0
}

// ���������Ե��ұߵ�һ��Ԫ�ؽ�������Сֵ��ѯ��Χ
TEST(RMQ, MinQueryEndsAtLast) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data);
    int n = (int)int_test_data.size(); // ��ȡ�����С

    ASSERT_TRUE(rmq.query(n - 3, n - 1).has_value());
    EXPECT_EQ(rmq.query(n - 3, n - 1).value(), 7); // {9, 7, 8} -> 7

    ASSERT_TRUE(rmq.query(n - 4, n - 1).has_value());
    EXPECT_EQ(rmq.query(n - 4, n - 1).value(), 0); // {0, 9, 7, 8} -> 0

    ASSERT_TRUE(rmq.query(5, n - 1).has_value());
    EXPECT_EQ(rmq.query(5, n - 1).value(), 0); // {6, 0, 9, 7, 8} -> 0
}

// ���Ը������������Сֵ��ѯ
TEST(RMQ, DoubleMinQuery) {
    std::vector<double> double_test_data = {3.14, 2.71, 1.618, 9.8};
    RMQ<double> rmq(double_test_data); // Ĭ�� std::less<double>

    ASSERT_TRUE(rmq.query(0, 3).has_value());
    EXPECT_DOUBLE_EQ(rmq.query(0, 3).value(), 1.618); // {3.14, 2.71, 1.618, 9.8} -> 1.618

    ASSERT_TRUE(rmq.query(0, 1).has_value());
    EXPECT_DOUBLE_EQ(rmq.query(0, 1).value(), 2.71); // {3.14, 2.71} -> 2.71
}

// �����������ַ����������Сֵ��ѯ
TEST(RMQ, StringMinQuery) {
    std::vector<std::string> string_test_data = {"banana", "apple", "cherry", "date", "apricot"};
    RMQ<std::string> rmq(string_test_data); // Ĭ�� std::less<std::string> (���ֵ���)

    ASSERT_TRUE(rmq.query(0, 4).has_value());
    EXPECT_EQ(rmq.query(0, 4).value(), "apple"); // ����������Сֵ (�ֵ�����С)

    ASSERT_TRUE(rmq.query(0, 2).has_value());
    EXPECT_EQ(rmq.query(0, 2).value(), "apple"); // {"banana", "apple", "cherry"} -> "apple"

    ASSERT_TRUE(rmq.query(2, 4).has_value());
    EXPECT_EQ(rmq.query(2, 4).value(), "apricot"); // {"cherry", "date", "apricot"} -> "apricot"

    ASSERT_TRUE(rmq.query(3, 3).has_value());
    EXPECT_EQ(rmq.query(3, 3).value(), "date"); // ����Ԫ�� {"date"} -> "date"
}

// ���������Դ���ߵ�һ��Ԫ�ؿ�ʼ���ַ�����Сֵ��ѯ��Χ
TEST(RMQ, StringMinQueryStartsFromZero) {
    std::vector<std::string> string_test_data = {"zebra", "xylophone", "apple", "grape", "banana"};
    RMQ<std::string> rmq(string_test_data);

    ASSERT_TRUE(rmq.query(0, 2).has_value());
    EXPECT_EQ(rmq.query(0, 2).value(), "apple"); // {"zebra", "xylophone", "apple"} -> "apple"

    ASSERT_TRUE(rmq.query(0, 4).has_value());
    EXPECT_EQ(rmq.query(0, 4).value(), "apple"); // {"zebra", "xylophone", "apple", "grape", "banana"} -> "apple"
}

// ���������Ե��ұߵ�һ��Ԫ�ؽ������ַ�����Сֵ��ѯ��Χ
TEST(RMQ, StringMinQueryEndsAtLast) {
    std::vector<std::string> string_test_data = {"orange", "apple", "grape", "kiwi", "fig"};
    RMQ<std::string> rmq(string_test_data);
    int n = (int)string_test_data.size();

    ASSERT_TRUE(rmq.query(n - 3, n - 1).has_value());
    EXPECT_EQ(rmq.query(n - 3, n - 1).value(), "fig"); // {"grape", "kiwi", "fig"} -> "fig"

    ASSERT_TRUE(rmq.query(1, n - 1).has_value());
    EXPECT_EQ(rmq.query(1, n - 1).value(), "apple"); // {"apple", "grape", "kiwi", "fig"} -> "apple"
}


// ������Ч��ѯ��Χ���������� std::nullopt
TEST(RMQ, InvalidMinQueryRanges) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data);

    ASSERT_FALSE(rmq.query(-1, 5).has_value());  // L < 0
    ASSERT_FALSE(rmq.query(0, 10).has_value()); // R >= n
    ASSERT_FALSE(rmq.query(5, 1).has_value());  // L > R
    ASSERT_FALSE(rmq.query(10, 10).has_value()); // L, R >= n
}

// --- ���ֵ��ѯ���� (ʹ�� std::greater<T>) ---

// ����������������ֵ��ѯ
TEST(RMQ, IntegerMaxQuery) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int, std::greater<int>> rmq_max(int_test_data); // ʹ�� std::greater<int>

    // ���Ͳ�ѯ
    ASSERT_TRUE(rmq_max.query(0, 9).has_value());
    EXPECT_EQ(rmq_max.query(0, 9).value(), 9); // �����������ֵ

    ASSERT_TRUE(rmq_max.query(1, 5).has_value());
    EXPECT_EQ(rmq_max.query(1, 5).value(), 6); // {3, 2, 5, 4, 6} -> 6

    ASSERT_TRUE(rmq_max.query(6, 6).has_value());
    EXPECT_EQ(rmq_max.query(6, 6).value(), 0); // ����Ԫ�� {0} -> 0

    ASSERT_TRUE(rmq_max.query(0, 0).has_value());
    EXPECT_EQ(rmq_max.query(0, 0).value(), 1); // ����Ԫ�� {1} -> 1

    ASSERT_TRUE(rmq_max.query(7, 8).has_value());
    EXPECT_EQ(rmq_max.query(7, 8).value(), 9); // {9, 7} -> 9
}

// ���������Դ���ߵ�һ��Ԫ�ؿ�ʼ�����ֵ��ѯ��Χ
TEST(RMQ, MaxQueryStartsFromZero) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int, std::greater<int>> rmq_max(int_test_data);

    ASSERT_TRUE(rmq_max.query(0, 2).has_value());
    EXPECT_EQ(rmq_max.query(0, 2).value(), 3); // {1, 3, 2} -> 3

    ASSERT_TRUE(rmq_max.query(0, 3).has_value());
    EXPECT_EQ(rmq_max.query(0, 3).value(), 5); // {1, 3, 2, 5} -> 5

    ASSERT_TRUE(rmq_max.query(0, 6).has_value());
    EXPECT_EQ(rmq_max.query(0, 6).value(), 6); // {1, 3, 2, 5, 4, 6, 0} -> 6
}

// ���������Ե��ұߵ�һ��Ԫ�ؽ��������ֵ��ѯ��Χ
TEST(RMQ, MaxQueryEndsAtLast) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int, std::greater<int>> rmq_max(int_test_data);
    int n = (int)int_test_data.size(); // ��ȡ�����С

    ASSERT_TRUE(rmq_max.query(n - 3, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(n - 3, n - 1).value(), 9); // {9, 7, 8} -> 9

    ASSERT_TRUE(rmq_max.query(n - 4, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(n - 4, n - 1).value(), 9); // {0, 9, 7, 8} -> 9

    ASSERT_TRUE(rmq_max.query(5, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(5, n - 1).value(), 9); // {6, 0, 9, 7, 8} -> 9
}

// ���Ը�������������ֵ��ѯ
TEST(RMQ, DoubleMaxQuery) {
    std::vector<double> double_test_data = {3.14, 2.71, 1.618, 9.8};
    RMQ<double, std::greater<double>> rmq_max(double_test_data); // ʹ�� std::greater<double>

    ASSERT_TRUE(rmq_max.query(0, 3).has_value());
    EXPECT_DOUBLE_EQ(rmq_max.query(0, 3).value(), 9.8); // {3.14, 2.71, 1.618, 9.8} -> 9.8

    ASSERT_TRUE(rmq_max.query(0, 1).has_value());
    EXPECT_DOUBLE_EQ(rmq_max.query(0, 1).value(), 3.14); // {3.14, 2.71} -> 3.14
}

// �����������ַ�����������ֵ��ѯ
TEST(RMQ, StringMaxQuery) {
    std::vector<std::string> string_test_data = {"banana", "apple", "cherry", "date", "apricot"};
    RMQ<std::string, std::greater<std::string>> rmq_max(string_test_data); // ʹ�� std::greater<std::string> (���ֵ���)

    ASSERT_TRUE(rmq_max.query(0, 4).has_value());
    EXPECT_EQ(rmq_max.query(0, 4).value(), "date"); // �����������ֵ (�ֵ������)

    ASSERT_TRUE(rmq_max.query(0, 2).has_value());
    EXPECT_EQ(rmq_max.query(0, 2).value(), "cherry"); // {"banana", "apple", "cherry"} -> "cherry"

    ASSERT_TRUE(rmq_max.query(2, 4).has_value());
    EXPECT_EQ(rmq_max.query(2, 4).value(), "date"); // {"cherry", "date", "apricot"} -> "date"

    ASSERT_TRUE(rmq_max.query(3, 3).has_value());
    EXPECT_EQ(rmq_max.query(3, 3).value(), "date"); // ����Ԫ�� {"date"} -> "date"
}

// ���������Դ���ߵ�һ��Ԫ�ؿ�ʼ���ַ������ֵ��ѯ��Χ
TEST(RMQ, StringMaxQueryStartsFromZero) {
    std::vector<std::string> string_test_data = {"zebra", "xylophone", "apple", "grape", "banana"};
    RMQ<std::string, std::greater<std::string>> rmq_max(string_test_data);

    ASSERT_TRUE(rmq_max.query(0, 2).has_value());
    EXPECT_EQ(rmq_max.query(0, 2).value(), "zebra"); // {"zebra", "xylophone", "apple"} -> "zebra"

    ASSERT_TRUE(rmq_max.query(0, 4).has_value());
    EXPECT_EQ(rmq_max.query(0, 4).value(), "zebra"); // {"zebra", "xylophone", "apple", "grape", "banana"} -> "zebra"
}

// ���������Ե��ұߵ�һ��Ԫ�ؽ������ַ������ֵ��ѯ��Χ
TEST(RMQ, StringMaxQueryEndsAtLast) {
    std::vector<std::string> string_test_data = {"orange", "apple", "grape", "kiwi", "fig"};
    RMQ<std::string, std::greater<std::string>> rmq_max(string_test_data);
    int n = (int)string_test_data.size();

    ASSERT_TRUE(rmq_max.query(n - 3, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(n - 3, n - 1).value(), "kiwi"); // {"grape", "kiwi", "fig"} -> "kiwi"

    ASSERT_TRUE(rmq_max.query(1, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(1, n - 1).value(), "kiwi"); // {"apple", "grape", "kiwi", "fig"} -> "kiwi"
}

// ������Ч��ѯ��Χ���������� std::nullopt
TEST(RMQ, InvalidMaxQueryRanges) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int, std::greater<int>> rmq_max(int_test_data);

    ASSERT_FALSE(rmq_max.query(-1, 5).has_value());
    ASSERT_FALSE(rmq_max.query(0, 10).has_value());
    ASSERT_FALSE(rmq_max.query(5, 1).has_value());
}

// --- clear() �������� ---

TEST(RMQ, ClearMethod) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data);
    ASSERT_TRUE(rmq.query(0, 9).has_value()); // ȷ�����ݴ���

    rmq.clear();
    // ��պ󣬲�ѯӦ�÷��� std::nullopt����Ϊ n ��Ϊ 0
    ASSERT_FALSE(rmq.query(0, 0).has_value());

    // �������µ����ݹ���
    std::vector<int> new_data = {10, 20, 5};
    RMQ<int> rmq_reinit(new_data);
    ASSERT_TRUE(rmq_reinit.query(0, 2).has_value());
    EXPECT_EQ(rmq_reinit.query(0, 2).value(), 5);
}


// --- �Զ���ṹ��ͱȽ������� ---

// ����һ���򵥵� Point �ṹ��
struct Point {
    int x;
    int y;

    // Ϊ�˷����ӡ�ͱȽϣ��������� operator==
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// Ϊ Point �ṹ���ṩһ����ӡ���������� Google Test ���ʧ����Ϣ
void PrintTo(const Point& p, std::ostream* os) {
    *os << "{" << p.x << ", " << p.y << "}";
}

// �Զ���Ƚ������Ȱ� x ��������x ��ͬ�� y �������� (������Сֵ)
struct PointMinComparator {
    bool operator()(const Point& a, const Point& b) const {
        if (a.x != b.x) {
            return a.x < b.x;
        }
        return a.y < b.y;
    }
};

// �Զ���Ƚ������Ȱ� x ���꽵��x ��ͬ�� y ���꽵�� (�������ֵ)
struct PointMaxComparator {
    bool operator()(const Point& a, const Point& b) const {
        if (a.x != b.x) {
            return a.x > b.x;
        }
        return a.y > b.y;
    }
};

// ���� Point �ṹ�����Сֵ��ѯ
TEST(RMQ, PointMinQuery) {
    std::vector<Point> points = {
        {10, 5},
        {5, 10},
        {10, 2},
        {1, 8},
        {5, 5}
    };
    RMQ<Point, PointMinComparator> rmq(points);

    ASSERT_TRUE(rmq.query(0, 4).has_value());
    EXPECT_EQ(rmq.query(0, 4).value(), (Point{1, 8})); // ������Сֵ

    ASSERT_TRUE(rmq.query(0, 2).has_value());
    EXPECT_EQ(rmq.query(0, 2).value(), (Point{5, 10})); // {10,5}, {5,10}, {10,2} -> {5,10}

    ASSERT_TRUE(rmq.query(2, 4).has_value());
    EXPECT_EQ(rmq.query(2, 4).value(), (Point{1, 8})); // {10,2}, {1,8}, {5,5} -> {1,8}

    ASSERT_TRUE(rmq.query(0, 0).has_value());
    EXPECT_EQ(rmq.query(0, 0).value(), (Point{10, 5})); // ����Ԫ��
}

// ���� Point �ṹ������ֵ��ѯ
TEST(RMQ, PointMaxQuery) {
    std::vector<Point> points = {
        {10, 5},
        {5, 10},
        {10, 2},
        {1, 8},
        {5, 5}
    };
    RMQ<Point, PointMaxComparator> rmq_max(points);

    ASSERT_TRUE(rmq_max.query(0, 4).has_value());
    EXPECT_EQ(rmq_max.query(0, 4).value(), (Point{10, 5})); // �������ֵ

    ASSERT_TRUE(rmq_max.query(0, 2).has_value());
    EXPECT_EQ(rmq_max.query(0, 2).value(), (Point{10, 5})); // {10,5}, {5,10}, {10,2} -> {10,5}

    ASSERT_TRUE(rmq_max.query(2, 4).has_value());
    EXPECT_EQ(rmq_max.query(2, 4).value(), (Point{10, 2})); // {10,2}, {1,8}, {5,5} -> {10,2}

    ASSERT_TRUE(rmq_max.query(0, 0).has_value());
    EXPECT_EQ(rmq_max.query(0, 0).value(), (Point{10, 5})); // ����Ԫ��
}