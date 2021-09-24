# UtilityLib
My c++ utility library, should be general and clean

标准库已有函数将尽量不出现（部分可能重复）
Function already known in STD should not appear again (not all)

1. Find kth element O(n)
    std::nth_element(begin, begin+k, end);
2. Enumerate Permutaion
    int q[] = { 0,1,2 };
    do
    {
        std::cout << q[0] << ' ' << q[1] << ' ' << q[2] << '\n';
    } while( std::next_permutation( q.begin(), q.end() ) );
3. max/min element_max/element_min
    std::max_element(...)
    std::min_element()
4. n进制转10进制
    char a="FF";
    int r = strtol( a, NULL, 16 );//255
5. 10进制转n进制
    int number = 10;
    char s[10];
    _itoa_s(number, s, 2);//1010