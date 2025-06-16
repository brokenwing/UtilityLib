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
        EXPECT_EQ( rmq.query( 0, 4 ).value(), 5 ); // 整个数组最大值
    }
    {
        RMQ<int, CMP> rmq( CMP( false ) );
        std::vector<int> data = { 1, 2, 3, 4, 5 };
        rmq.initialize( data );
        ASSERT_TRUE( rmq.query( 0, 4 ).has_value() );
        EXPECT_EQ( rmq.query( 0, 4 ).value(), 1 );
    }
}

// --- 最小值查询测试 (默认比较器 std::less<T>) ---

// 测试构造函数和空数组情况
TEST(RMQ, HandlesEmptyArray) {
    std::vector<int> empty_int_data = {};
    RMQ<int> rmq_empty_int(empty_int_data);
    ASSERT_FALSE(rmq_empty_int.query(0, 0).has_value());

    std::vector<std::string> empty_string_data = {};
    RMQ<std::string> rmq_empty_string(empty_string_data);
    ASSERT_FALSE(rmq_empty_string.query(0, 0).has_value());
}

// 测试单元素数组
TEST(RMQ, HandlesSingleElementArray) {
    std::vector<int> single_int_data = {42};
    RMQ<int> rmq_single_int(single_int_data);
    ASSERT_TRUE(rmq_single_int.query(0, 0).has_value());
    EXPECT_EQ(rmq_single_int.query(0, 0).value(), 42);
    ASSERT_FALSE(rmq_single_int.query(0, 1).has_value()); // 越界
    ASSERT_FALSE(rmq_single_int.query(-1, 0).has_value()); // 越界

    std::vector<std::string> single_string_data = {"apple"};
    RMQ<std::string> rmq_single_string(single_string_data);
    ASSERT_TRUE(rmq_single_string.query(0, 0).has_value());
    EXPECT_EQ(rmq_single_string.query(0, 0).value(), "apple");
    ASSERT_FALSE(rmq_single_string.query(0, 1).has_value()); // 越界
    ASSERT_FALSE(rmq_single_string.query(-1, 0).has_value()); // 越界
}

// 测试整数数组的最小值查询
TEST(RMQ, IntegerMinQuery) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data); // 默认 std::less<int>

    // 典型查询
    ASSERT_TRUE(rmq.query(0, 9).has_value());
    EXPECT_EQ(rmq.query(0, 9).value(), 0); // 整个数组最小值

    ASSERT_TRUE(rmq.query(1, 5).has_value());
    EXPECT_EQ(rmq.query(1, 5).value(), 2); // {3, 2, 5, 4, 6} -> 2

    ASSERT_TRUE(rmq.query(6, 6).has_value());
    EXPECT_EQ(rmq.query(6, 6).value(), 0); // 单个元素 {0} -> 0

    ASSERT_TRUE(rmq.query(0, 0).has_value());
    EXPECT_EQ(rmq.query(0, 0).value(), 1); // 单个元素 {1} -> 1

    ASSERT_TRUE(rmq.query(7, 8).has_value());
    EXPECT_EQ(rmq.query(7, 8).value(), 7); // {9, 7} -> 7
}

// 新增：测试从左边第一个元素开始的最小值查询范围
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

// 新增：测试到右边第一个元素结束的最小值查询范围
TEST(RMQ, MinQueryEndsAtLast) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data);
    int n = (int)int_test_data.size(); // 获取数组大小

    ASSERT_TRUE(rmq.query(n - 3, n - 1).has_value());
    EXPECT_EQ(rmq.query(n - 3, n - 1).value(), 7); // {9, 7, 8} -> 7

    ASSERT_TRUE(rmq.query(n - 4, n - 1).has_value());
    EXPECT_EQ(rmq.query(n - 4, n - 1).value(), 0); // {0, 9, 7, 8} -> 0

    ASSERT_TRUE(rmq.query(5, n - 1).has_value());
    EXPECT_EQ(rmq.query(5, n - 1).value(), 0); // {6, 0, 9, 7, 8} -> 0
}

// 测试浮点数数组的最小值查询
TEST(RMQ, DoubleMinQuery) {
    std::vector<double> double_test_data = {3.14, 2.71, 1.618, 9.8};
    RMQ<double> rmq(double_test_data); // 默认 std::less<double>

    ASSERT_TRUE(rmq.query(0, 3).has_value());
    EXPECT_DOUBLE_EQ(rmq.query(0, 3).value(), 1.618); // {3.14, 2.71, 1.618, 9.8} -> 1.618

    ASSERT_TRUE(rmq.query(0, 1).has_value());
    EXPECT_DOUBLE_EQ(rmq.query(0, 1).value(), 2.71); // {3.14, 2.71} -> 2.71
}

// 新增：测试字符串数组的最小值查询
TEST(RMQ, StringMinQuery) {
    std::vector<std::string> string_test_data = {"banana", "apple", "cherry", "date", "apricot"};
    RMQ<std::string> rmq(string_test_data); // 默认 std::less<std::string> (按字典序)

    ASSERT_TRUE(rmq.query(0, 4).has_value());
    EXPECT_EQ(rmq.query(0, 4).value(), "apple"); // 整个数组最小值 (字典序最小)

    ASSERT_TRUE(rmq.query(0, 2).has_value());
    EXPECT_EQ(rmq.query(0, 2).value(), "apple"); // {"banana", "apple", "cherry"} -> "apple"

    ASSERT_TRUE(rmq.query(2, 4).has_value());
    EXPECT_EQ(rmq.query(2, 4).value(), "apricot"); // {"cherry", "date", "apricot"} -> "apricot"

    ASSERT_TRUE(rmq.query(3, 3).has_value());
    EXPECT_EQ(rmq.query(3, 3).value(), "date"); // 单个元素 {"date"} -> "date"
}

// 新增：测试从左边第一个元素开始的字符串最小值查询范围
TEST(RMQ, StringMinQueryStartsFromZero) {
    std::vector<std::string> string_test_data = {"zebra", "xylophone", "apple", "grape", "banana"};
    RMQ<std::string> rmq(string_test_data);

    ASSERT_TRUE(rmq.query(0, 2).has_value());
    EXPECT_EQ(rmq.query(0, 2).value(), "apple"); // {"zebra", "xylophone", "apple"} -> "apple"

    ASSERT_TRUE(rmq.query(0, 4).has_value());
    EXPECT_EQ(rmq.query(0, 4).value(), "apple"); // {"zebra", "xylophone", "apple", "grape", "banana"} -> "apple"
}

// 新增：测试到右边第一个元素结束的字符串最小值查询范围
TEST(RMQ, StringMinQueryEndsAtLast) {
    std::vector<std::string> string_test_data = {"orange", "apple", "grape", "kiwi", "fig"};
    RMQ<std::string> rmq(string_test_data);
    int n = (int)string_test_data.size();

    ASSERT_TRUE(rmq.query(n - 3, n - 1).has_value());
    EXPECT_EQ(rmq.query(n - 3, n - 1).value(), "fig"); // {"grape", "kiwi", "fig"} -> "fig"

    ASSERT_TRUE(rmq.query(1, n - 1).has_value());
    EXPECT_EQ(rmq.query(1, n - 1).value(), "apple"); // {"apple", "grape", "kiwi", "fig"} -> "apple"
}


// 测试无效查询范围，期望返回 std::nullopt
TEST(RMQ, InvalidMinQueryRanges) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data);

    ASSERT_FALSE(rmq.query(-1, 5).has_value());  // L < 0
    ASSERT_FALSE(rmq.query(0, 10).has_value()); // R >= n
    ASSERT_FALSE(rmq.query(5, 1).has_value());  // L > R
    ASSERT_FALSE(rmq.query(10, 10).has_value()); // L, R >= n
}

// --- 最大值查询测试 (使用 std::greater<T>) ---

// 测试整数数组的最大值查询
TEST(RMQ, IntegerMaxQuery) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int, std::greater<int>> rmq_max(int_test_data); // 使用 std::greater<int>

    // 典型查询
    ASSERT_TRUE(rmq_max.query(0, 9).has_value());
    EXPECT_EQ(rmq_max.query(0, 9).value(), 9); // 整个数组最大值

    ASSERT_TRUE(rmq_max.query(1, 5).has_value());
    EXPECT_EQ(rmq_max.query(1, 5).value(), 6); // {3, 2, 5, 4, 6} -> 6

    ASSERT_TRUE(rmq_max.query(6, 6).has_value());
    EXPECT_EQ(rmq_max.query(6, 6).value(), 0); // 单个元素 {0} -> 0

    ASSERT_TRUE(rmq_max.query(0, 0).has_value());
    EXPECT_EQ(rmq_max.query(0, 0).value(), 1); // 单个元素 {1} -> 1

    ASSERT_TRUE(rmq_max.query(7, 8).has_value());
    EXPECT_EQ(rmq_max.query(7, 8).value(), 9); // {9, 7} -> 9
}

// 新增：测试从左边第一个元素开始的最大值查询范围
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

// 新增：测试到右边第一个元素结束的最大值查询范围
TEST(RMQ, MaxQueryEndsAtLast) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int, std::greater<int>> rmq_max(int_test_data);
    int n = (int)int_test_data.size(); // 获取数组大小

    ASSERT_TRUE(rmq_max.query(n - 3, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(n - 3, n - 1).value(), 9); // {9, 7, 8} -> 9

    ASSERT_TRUE(rmq_max.query(n - 4, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(n - 4, n - 1).value(), 9); // {0, 9, 7, 8} -> 9

    ASSERT_TRUE(rmq_max.query(5, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(5, n - 1).value(), 9); // {6, 0, 9, 7, 8} -> 9
}

// 测试浮点数数组的最大值查询
TEST(RMQ, DoubleMaxQuery) {
    std::vector<double> double_test_data = {3.14, 2.71, 1.618, 9.8};
    RMQ<double, std::greater<double>> rmq_max(double_test_data); // 使用 std::greater<double>

    ASSERT_TRUE(rmq_max.query(0, 3).has_value());
    EXPECT_DOUBLE_EQ(rmq_max.query(0, 3).value(), 9.8); // {3.14, 2.71, 1.618, 9.8} -> 9.8

    ASSERT_TRUE(rmq_max.query(0, 1).has_value());
    EXPECT_DOUBLE_EQ(rmq_max.query(0, 1).value(), 3.14); // {3.14, 2.71} -> 3.14
}

// 新增：测试字符串数组的最大值查询
TEST(RMQ, StringMaxQuery) {
    std::vector<std::string> string_test_data = {"banana", "apple", "cherry", "date", "apricot"};
    RMQ<std::string, std::greater<std::string>> rmq_max(string_test_data); // 使用 std::greater<std::string> (按字典序)

    ASSERT_TRUE(rmq_max.query(0, 4).has_value());
    EXPECT_EQ(rmq_max.query(0, 4).value(), "date"); // 整个数组最大值 (字典序最大)

    ASSERT_TRUE(rmq_max.query(0, 2).has_value());
    EXPECT_EQ(rmq_max.query(0, 2).value(), "cherry"); // {"banana", "apple", "cherry"} -> "cherry"

    ASSERT_TRUE(rmq_max.query(2, 4).has_value());
    EXPECT_EQ(rmq_max.query(2, 4).value(), "date"); // {"cherry", "date", "apricot"} -> "date"

    ASSERT_TRUE(rmq_max.query(3, 3).has_value());
    EXPECT_EQ(rmq_max.query(3, 3).value(), "date"); // 单个元素 {"date"} -> "date"
}

// 新增：测试从左边第一个元素开始的字符串最大值查询范围
TEST(RMQ, StringMaxQueryStartsFromZero) {
    std::vector<std::string> string_test_data = {"zebra", "xylophone", "apple", "grape", "banana"};
    RMQ<std::string, std::greater<std::string>> rmq_max(string_test_data);

    ASSERT_TRUE(rmq_max.query(0, 2).has_value());
    EXPECT_EQ(rmq_max.query(0, 2).value(), "zebra"); // {"zebra", "xylophone", "apple"} -> "zebra"

    ASSERT_TRUE(rmq_max.query(0, 4).has_value());
    EXPECT_EQ(rmq_max.query(0, 4).value(), "zebra"); // {"zebra", "xylophone", "apple", "grape", "banana"} -> "zebra"
}

// 新增：测试到右边第一个元素结束的字符串最大值查询范围
TEST(RMQ, StringMaxQueryEndsAtLast) {
    std::vector<std::string> string_test_data = {"orange", "apple", "grape", "kiwi", "fig"};
    RMQ<std::string, std::greater<std::string>> rmq_max(string_test_data);
    int n = (int)string_test_data.size();

    ASSERT_TRUE(rmq_max.query(n - 3, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(n - 3, n - 1).value(), "kiwi"); // {"grape", "kiwi", "fig"} -> "kiwi"

    ASSERT_TRUE(rmq_max.query(1, n - 1).has_value());
    EXPECT_EQ(rmq_max.query(1, n - 1).value(), "kiwi"); // {"apple", "grape", "kiwi", "fig"} -> "kiwi"
}

// 测试无效查询范围，期望返回 std::nullopt
TEST(RMQ, InvalidMaxQueryRanges) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int, std::greater<int>> rmq_max(int_test_data);

    ASSERT_FALSE(rmq_max.query(-1, 5).has_value());
    ASSERT_FALSE(rmq_max.query(0, 10).has_value());
    ASSERT_FALSE(rmq_max.query(5, 1).has_value());
}

// --- clear() 方法测试 ---

TEST(RMQ, ClearMethod) {
    std::vector<int> int_test_data = {1, 3, 2, 5, 4, 6, 0, 9, 7, 8};
    RMQ<int> rmq(int_test_data);
    ASSERT_TRUE(rmq.query(0, 9).has_value()); // 确认数据存在

    rmq.clear();
    // 清空后，查询应该返回 std::nullopt，因为 n 变为 0
    ASSERT_FALSE(rmq.query(0, 0).has_value());

    // 重新用新的数据构造
    std::vector<int> new_data = {10, 20, 5};
    RMQ<int> rmq_reinit(new_data);
    ASSERT_TRUE(rmq_reinit.query(0, 2).has_value());
    EXPECT_EQ(rmq_reinit.query(0, 2).value(), 5);
}


// --- 自定义结构体和比较器测试 ---

// 定义一个简单的 Point 结构体
struct Point {
    int x;
    int y;

    // 为了方便打印和比较，可以重载 operator==
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// 为 Point 结构体提供一个打印函数，方便 Google Test 输出失败信息
void PrintTo(const Point& p, std::ostream* os) {
    *os << "{" << p.x << ", " << p.y << "}";
}

// 自定义比较器：先按 x 坐标升序，x 相同则按 y 坐标升序 (用于最小值)
struct PointMinComparator {
    bool operator()(const Point& a, const Point& b) const {
        if (a.x != b.x) {
            return a.x < b.x;
        }
        return a.y < b.y;
    }
};

// 自定义比较器：先按 x 坐标降序，x 相同则按 y 坐标降序 (用于最大值)
struct PointMaxComparator {
    bool operator()(const Point& a, const Point& b) const {
        if (a.x != b.x) {
            return a.x > b.x;
        }
        return a.y > b.y;
    }
};

// 测试 Point 结构体的最小值查询
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
    EXPECT_EQ(rmq.query(0, 4).value(), (Point{1, 8})); // 整体最小值

    ASSERT_TRUE(rmq.query(0, 2).has_value());
    EXPECT_EQ(rmq.query(0, 2).value(), (Point{5, 10})); // {10,5}, {5,10}, {10,2} -> {5,10}

    ASSERT_TRUE(rmq.query(2, 4).has_value());
    EXPECT_EQ(rmq.query(2, 4).value(), (Point{1, 8})); // {10,2}, {1,8}, {5,5} -> {1,8}

    ASSERT_TRUE(rmq.query(0, 0).has_value());
    EXPECT_EQ(rmq.query(0, 0).value(), (Point{10, 5})); // 单个元素
}

// 测试 Point 结构体的最大值查询
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
    EXPECT_EQ(rmq_max.query(0, 4).value(), (Point{10, 5})); // 整体最大值

    ASSERT_TRUE(rmq_max.query(0, 2).has_value());
    EXPECT_EQ(rmq_max.query(0, 2).value(), (Point{10, 5})); // {10,5}, {5,10}, {10,2} -> {10,5}

    ASSERT_TRUE(rmq_max.query(2, 4).has_value());
    EXPECT_EQ(rmq_max.query(2, 4).value(), (Point{10, 2})); // {10,2}, {1,8}, {5,5} -> {10,2}

    ASSERT_TRUE(rmq_max.query(0, 0).has_value());
    EXPECT_EQ(rmq_max.query(0, 0).value(), (Point{10, 5})); // 单个元素
}