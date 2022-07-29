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
#include "low_fragmentation_allocator.h"

#define FOR(i,a,b) for(int i=(a); (i)<(b); (i)++)
//#define FOR(i,a,b,step) for(int i=(a); (i)<(b); (i)+=(step))

#ifdef _DEBUG
#define RELEASE_VER_TRY
#define RELEASE_VER_CATCH_START( toCatch )
#define RELEASE_VER_CATCH_CONTENT( contents )
#define RELEASE_VER_CATCH_END
#else
#define RELEASE_VER_TRY try {
#define RELEASE_VER_CATCH_START( toCatch ) }catch( toCatch ){
#define RELEASE_VER_CATCH_CONTENT( contents ) contents;
#define RELEASE_VER_CATCH_END }
#endif

template<typename T>
using DefaultAllocator = LFA::low_fragmentation_allocator<T>;

namespace LFA
{
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
}


namespace Util
{
constexpr double eps = 1e-6;
//Nondeterministic Random Number Generator
typedef std::random_device NRNG;
//Pseudorandom number generator
typedef std::mt19937 RNG;
typedef std::mt19937_64 RNG64;
}
