//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_MEM_SLAB_HPP
#define ZPP_INCLUDE_ZPP_MEM_SLAB_HPP

#include <kernel.h>
#include <sys/__assert.h>

namespace zpp {

///
/// @brief Allocator that uses k_mem_slab for memory
///
/// @param Type the type this Allocator supports
///
template <class Type>
class borrowed_mem_slab {
public:
	using value_type = Type;
	using pointer = Type*;
	using const_pointer = const pointer;
	using size_type = uint32_t;
public:
	borrowed_mem_slab() = delete;

	///
	/// @brief create borrowed_mem_slab using an excisting k_mem_slab
	///
	/// @param mem_slab to use for memory allocation
	///
	constexpr borrowed_mem_slab(k_mem_slab* mem_slab) noexcept
		: m_mem_slab(mem_slab)
	{
		__ASSERT_NO_MSG(m_mem_slab != nullptr);
		__ASSERT_NO_MSG(m_mem_slab->block_size >= sizeof(value_type));
	}

	template <class U>
	constexpr borrowed_mem_slab(const borrowed_mem_slab<U>& other) noexcept
		: m_mem_slab(other.m_mem_slab)
	{
		__ASSERT_NO_MSG(m_mem_slab != nullptr);
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
		__ASSERT_NO_MSG(m_mem_slab != nullptr);
		__ASSERT_NO_MSG(n <= max_size());

		void* vp{nullptr};

		auto rc = k_mem_slab_alloc(m_mem_slab, &vp, K_FOREVER);

		if (rc == 0) {
			return static_cast<pointer>(vp);
		} else {
			return nullptr;
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
		__ASSERT_NO_MSG(m_mem_slab != nullptr);
		__ASSERT_NO_MSG(p != nullptr);

		void* vp = p;
		k_mem_slab_free(m_mem_slab, &vp);
	}

	///
	/// @brief get number Type's that can be allocated
	///
	/// @return the max number of Types that can be allocated
	///
	constexpr size_type max_size() const noexcept
	{
		__ASSERT_NO_MSG(m_mem_slab != nullptr);

		return m_mem_slab->block_size / sizeof(value_type);
	}

	///
	/// @brief get maxium number of blocks that can be allocated
	///
	/// @return the maxium number of blocks that can be allocated
	///
	constexpr auto total_block_count() noexcept {
		__ASSERT_NO_MSG(m_mem_slab != nullptr);

		return m_mem_slab->num_blocks;
	}

	///
	/// @brief get current number of used blocks
	///
	/// @return the current number of used blocks
	///
	constexpr auto used_block_count() noexcept {
		__ASSERT_NO_MSG(m_mem_slab != nullptr);

		return k_mem_slab_num_used_get(m_mem_slab);
	}

	///
	/// @brief get current number of free blocks
	///
	/// @return the current number of free blocks
	///
	constexpr auto free_block_count() noexcept
	{
		__ASSERT_NO_MSG(m_mem_slab != nullptr);

		return k_mem_slab_num_free_get(m_mem_slab);
	}
private:
	k_mem_slab*	m_mem_slab { nullptr };
};


///
/// @brief compare if two borrowed_mem_pool can be mixed
///
/// @return always false
///
template <class T, class U>
constexpr bool
operator==(const borrowed_mem_slab<T>&, const borrowed_mem_slab<U>&) noexcept
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
operator!=(const borrowed_mem_slab<T>&, const borrowed_mem_slab<U>&) noexcept
{
	return true;
}

template <typename Type, size_t ContCount>
constexpr size_t mem_slab_block_size() noexcept
{
	static_assert(ContCount > 0);

	constexpr auto block_size = sizeof(Type) * ContCount;

	// must be a multiple of 4
	return ((block_size + 3) / 4) * 4;
}

template <typename Type, size_t ContCount, size_t MemSize>
constexpr size_t mem_slab_block_count() noexcept
{
	constexpr auto block_size = mem_slab_block_size<Type, ContCount>();

	static_assert(MemSize >= block_size);

	return MemSize / block_size;
}

} // namespace zpp


///
/// @brief define a borrowed_mem_slab
///
/// @param name the name of the object
/// @param Type the type of the objects in the pool
/// @param ContCount the continuous number of Type's that are supported
/// @param MemSize the size of the pool in bytes
///
#define ZPP_MEM_SLAB_DEFINE(name, Type, ContCount, MemSize) \
	\
	namespace { \
	constexpr auto block_size_##name = ::zpp::mem_slab_block_size< \
							Type, \
							ContCount>(); \
	constexpr auto block_count_##name = ::zpp::mem_slab_block_count< \
							Type, \
							ContCount, \
							MemSize>(); \
	K_MEM_SLAB_DEFINE(native_##name, \
		block_size_##name, \
		block_count_##name, \
		alignof(Type)); \
	} \
	::zpp::borrowed_mem_slab<Type> name(&native_##name)

#endif // ZPP_INCLUDE_ZPP_MEM_SLAB_HPP
