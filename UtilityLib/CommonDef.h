#pragma once
#include <random>
#include <vector>
#include <list>
#include <stack>
#include <deque>
#include <queue>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <sstream>

namespace LFA
{
template<typename T>
using DefaultAllocator = std::allocator<T>;//LowFragmentationAllocator<T>;

template<typename T>
using deque = std::deque<T, DefaultAllocator<T>>;

template<typename T>
using list = std::list<T, DefaultAllocator<T>>;

template<typename T, typename Cont = deque<T>>
using queue = std::queue<T, Cont>;

template<typename T, typename Cont = deque<T>>
using stack = std::stack<T, Cont>;

template<typename T>
using vector = std::vector<T, DefaultAllocator<T>>;

template<typename T, typename Pr = std::less<T>>
using set = std::set<T, Pr, DefaultAllocator<T>>;

template<typename K, typename V, typename Pr = std::less<K>>
using map = std::map<K, V, Pr, DefaultAllocator<std::pair<const K, V>>>;

template<typename K, typename V, typename Pr = std::less<K>>
using multimap = std::multimap<K, V, Pr, DefaultAllocator<std::pair<const K, V>>>;

template<typename K, typename Hasher = std::hash<K>, typename KeyEq = std::equal_to<K>>
using unordered_set = std::unordered_set<K, Hasher, KeyEq, DefaultAllocator<K>>;

template<typename K, typename V, typename Hasher = std::hash<K>, typename KeyEq = std::equal_to<K>>
using unordered_map = std::unordered_map< K, V, Hasher, KeyEq, DefaultAllocator<std::pair<const K, V>>>;

template<typename K, typename V, typename Hasher = std::hash<K>, typename KeyEq = std::equal_to<K>>
using unordered_multimap = std::unordered_multimap< K, V, Hasher, KeyEq, DefaultAllocator<std::pair<const K, V>>>;

using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, DefaultAllocator<wchar_t>>;
using wstringstream = std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, DefaultAllocator<wchar_t>>;
using owstringstream = std::basic_ostringstream<wchar_t, std::char_traits<wchar_t>, DefaultAllocator<wchar_t>>;

using string = std::basic_string<char, std::char_traits<char>, DefaultAllocator<char>>;
using stringstream = std::basic_stringstream<char, std::char_traits<char>, DefaultAllocator<char>>;
using ostringstream = std::basic_ostringstream<char, std::char_traits<char>, DefaultAllocator<char>>;
using istringstream = std::basic_istringstream<char, std::char_traits<char>, DefaultAllocator<char>>;

#ifdef _UNICODE
	typedef wstring _tstring;
	typedef wstringstream _tstringstream;
	typedef owstringstream _tostringstream;
	typedef std::wofstream _tofstream;
	typedef std::wifstream _tifstream;
#else
	typedef string _tstring;
	typedef stringstream _tstringstream;
	typedef ostringstream _tostringstream;
	typedef std::ofstream _tofstream;
	typedef std::ifstream _tifstream;
#endif
}


namespace Util
{
constexpr double eps = 1e-6;
typedef std::mt19937 RNG;
typedef std::mt19937_64 RNG64;
}