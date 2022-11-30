#pragma once
#include <xutility>

namespace Util
{
template <typename T, typename _Comp, typename _Alloc, bool isMulti>
class _Tset_traits
{
public:
	using key_type = T;
	using value_type = T;
	using allocator_type = _Alloc;
	using key_compare_type = _Comp;
	using value_compare_type = _Comp;

	static constexpr bool MultiSet = isMulti;

	static const key_type& get_key( const value_type& val )
	{
		return val;
	}
	struct key_equal_type
	{
		key_compare_type cp;
		bool operator()( const key_type& a, const key_type& b )const
		{
			return !( cp( a, b ) || cp( b, a ) );
		}
	};
};

template <typename _Key, typename _Value, typename _Comp, typename _Alloc, bool isMulti>
class _Tmap_traits
{
public:
	class mappair :private std::pair<_Key, _Value>
	{
	private:
		using _Mybase = std::pair<_Key, _Value>;
	public:
		friend class _Tmap_traits;
		using _Mybase::_Mybase;

		const _Key& key()const noexcept
		{
			return this->first;
		}
		const _Value& value()const noexcept
		{
			return this->second;
		}
		_Value& value()noexcept
		{
			return this->second;
		}
	};

	using key_type = _Key;
	using value_type = mappair;
	using allocator_type = std::allocator_traits<_Alloc>::template rebind_alloc<value_type>;
	using key_compare_type = _Comp;

	static constexpr bool MultiSet = isMulti;
	
	static const key_type& get_key( const value_type& val )
	{
		return val.first;
	}
	struct value_compare_type
	{
		key_compare_type cp;
		bool operator()( const value_type& a, const value_type& b )const
		{
			return cp( a.first, b.first );
		}
	};
	struct key_equal_type
	{
		key_compare_type cp;
		bool operator()( const key_type& a, const key_type& b )const
		{
			return !( cp( a, b ) || cp( b, a ) );
		}
	};
};
}