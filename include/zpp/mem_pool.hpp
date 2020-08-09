//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_MEM_POOL_HPP
#define ZPP_INCLUDE_ZPP_MEM_POOL_HPP

#include <kernel.h>
#include <sys/__assert.h>

namespace zpp {

///
/// @brief Allocator that uses k_mem_pool for memory
///
/// @param Type the type this Allocator supports
///
template <class Type>
class borrowed_mem_pool {
public:
	using value_type = Type;
	using pointer = Type*;
	using const_pointer = const pointer;
	using size_type = uint32_t;
public:
	borrowed_mem_pool() = delete;

	///
	/// @brief create borrowed_mem_pool using an excisting k_mem_pool
	///
	/// @param mem_pool to use for memory allocation
	///
	constexpr borrowed_mem_pool(k_mem_pool* mem_pool) noexcept
		: m_mem_pool(mem_pool)
	{
		__ASSERT_NO_MSG(m_mem_pool != nullptr);
	}

	template <class U>
	constexpr borrowed_mem_pool(const borrowed_mem_pool<U>& other) noexcept
		: m_mem_pool(other.m_mem_pool)
	{
		__ASSERT_NO_MSG(m_mem_pool != nullptr);
		__ASSERT_NO_MSG(max_size() > 0);
	}

	///
	/// @brief allocate memory for n times sizeof(Type)
	///
	/// @param n the number Types to alloc
	///
	/// @return pointer to memory for n times Type
	///
	[[nodiscard]] pointer allocate(size_type n) noexcept
	{
		__ASSERT_NO_MSG(m_mem_pool != nullptr);
		__ASSERT_NO_MSG(n <= max_size());

		auto vp = k_mem_pool_malloc(m_mem_pool, n * sizeof(value_type));
		if (vp == nullptr) {
			return nullptr;
		} else {
			return static_cast<pointer>(vp);
		}
	}

	///
	/// @brief deallocate memory
	///
	/// @param p the pointer to free
	/// @param n the number of Type (not used)
	///
	void deallocate(pointer p, size_type) noexcept
	{
		__ASSERT_NO_MSG(m_mem_pool != nullptr);
		__ASSERT_NO_MSG(p != nullptr);

		k_free(p);
	}

	///
	/// @brief get number Type's that can be allocated
	///
	/// @return the max number of Types that can be allocated
	///
	constexpr size_type max_size() const noexcept
	{
		__ASSERT_NO_MSG(m_mem_pool != nullptr);
#ifdef CONFIG_MEM_POOL_HEAP_BACKEND
		return m_mem_pool->heap->heap.init_bytes / sizeof(value_type);
#else
		return m_mem_pool->base.max_sz / sizeof(value_type);
#endif
	}
private:
	k_mem_pool*	m_mem_pool { nullptr };
};

///
/// @brief compare if two borrowed_mem_pool can be mixed
///
/// @return always false
///
template <class T, class U>
constexpr bool
operator==(const borrowed_mem_pool<T>&, const borrowed_mem_pool<U>&) noexcept
{
	return false;
}

///
/// @brief compare if two borrowed_mem_pool can be mixed
///
/// @return always true
///
template <class T, class U>
constexpr bool
operator!=(const borrowed_mem_pool<T>&, const borrowed_mem_pool<U>&) noexcept
{
	return true;
}

template <typename Type, size_t MinContCount>
constexpr size_t mem_pool_min_size() noexcept
{
	static_assert(MinContCount > 0);

	constexpr auto min_size = sizeof(Type) * MinContCount;

	// must be a multiple of 4
	return ((min_size + 3) / 4) * 4;
}

template <typename Type,
	  size_t MinContCount,
	  size_t MaxContCount>
constexpr size_t mem_pool_max_size() noexcept
{
	constexpr auto min_size = mem_pool_min_size<Type, MinContCount>();

	static_assert(MinContCount > 0);
	static_assert(MaxContCount > 0);
	static_assert(MaxContCount >= MinContCount);

	size_t max_size = min_size;

	while (max_size < (min_size * MaxContCount)) {
		max_size *= 4;
	}

	return max_size;
}

template <typename Type,
	  size_t MinContCount,
	  size_t MaxContCount,
	  size_t MemSize>
constexpr size_t mem_pool_block_count() noexcept
{
	constexpr auto max_size =
		mem_pool_max_size<Type, MinContCount, MaxContCount>();

	static_assert(MemSize >= max_size);

	return MemSize / max_size;
}

} // namespace zpp

///
/// @brief define a borrowed_mem_pool
///
/// @param name the name of the object
/// @param Type the type of the objects in the pool
/// @param MinContCount the minimum continuous number of Type's that are
///        supported
/// @param MaxContCount the maximun continuous number of Type's that are
///        supported
/// @param MemSize the size of the pool in bytes
///
#define ZPP_MEM_POOL_DEFINE(name, Type, MinContCount, MaxContCount, MemSize) \
	\
	namespace { \
	constexpr auto min_size_##name = ::zpp::mem_pool_min_size<Type, \
							MinContCount>(); \
	constexpr auto max_size_##name = ::zpp::mem_pool_max_size<Type, \
							MinContCount, \
							MaxContCount>(); \
	constexpr auto block_count_##name = ::zpp::mem_pool_block_count<Type, \
							MinContCount, \
							MaxContCount, \
							MemSize>(); \
	K_MEM_POOL_DEFINE(native_##name , \
			  min_size_##name, \
			  max_size_##name, \
			  block_count_##name, \
			  alignof(Type)); \
	} \
	::zpp::borrowed_mem_pool<Type> name(&native_##name)

#endif // ZPP_INCLUDE_ZPP_MEM_POOL_HPP
