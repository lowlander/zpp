//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP__INCLUDE__ZPP__MEM_SLAB_HPP
#define ZPP__INCLUDE__ZPP__MEM_SLAB_HPP

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <limits>
#include <new>
#include <memory>
#include <algorithm>

namespace zpp {

///
/// @brief compile time helper to calculate alignment
///
template<typename ItemType>
constexpr auto mem_slab_calc_item_alignment() noexcept
{
	auto start = alignof(void*);
	auto end = std::max(start, alignof(ItemType));

	while (start < end) {
		start *= 2;
	}

	return start;
}

///
/// @brief compile time helper to calculate size
///
template<typename ItemType>
constexpr auto mem_slab_calc_item_size() noexcept
{
	auto start = mem_slab_calc_item_alignment<ItemType>();
	auto end = std::max(start, sizeof(ItemType));

	while (start < end) {
		start += start;
	}

	return start;
}

///
/// @brief memeory slab
///
/// @param ItemType the type of the objects this slab produces
/// @param ItemCount the number of items this slab holds
/// @param ItemAlignment the alignment of the items
/// @param ItemSize the size of the items
///
template<typename ItemType,
	 size_t ItemCount,
	 size_t ItemAlignment = mem_slab_calc_item_alignment<ItemType>(),
	 size_t ItemSize = mem_slab_calc_item_size<ItemType>()>
class mem_slab
{
public:
	struct mem_slab_deleter {
		void operator()(ItemType* p) const noexcept {
			if (p != nullptr && m_slab != nullptr) {
				p->~ItemType();
				void* vp = (void*)p;
				k_mem_slab_free(m_slab->native_handle(), &vp);
			}
		}

		mem_slab* m_slab{ nullptr };
	};

	using unique_ptr = std::unique_ptr<ItemType, mem_slab_deleter>;
public:
	///
	/// @brief mem_slab constructor
	///
	mem_slab() noexcept
	{
		k_mem_slab_init(&m_slab, m_slab_buffer, ItemSize, ItemCount);
	}

	///
	/// @brief get native zephyr k_mem_slab handle
	///
	/// @return k_mem_slab pointer
	///
	constexpr auto native_handle() noexcept
	{
		return &m_slab;
	}

	///
	/// @brief alloc an ItemType waiting for ever if no block available
	///
	/// @param args the arguments to pass to the ItemType constructor
	///
	/// @return ItemType smart pointer or nulllptr on error
	///
	template<typename ...Args>
	[[nodiscard]] auto
	alloc(Args&& ...args) noexcept
	{
		return alloc_impl(K_FOREVER, std::forward<Args>(args)...);
	}

	///
	/// @brief alloc an ItemType without waiting
	///
	/// @param args the arguments to pass to the ItemType constructor
	///
	/// @return ItemType smart pointer or nulllptr on error
	///
	template<typename ...Args>
	[[nodiscard]] auto
	try_alloc(Args&& ...args) noexcept
	{
		return alloc_impl(K_NO_WAIT, std::forward<Args>(args)... );
	}

	///
	/// @brief alloc an ItemType waiting with a timeout
	///
	/// @param timeout the timeout time
	/// @param args the arguments to pass to the ItemType constructor
	///
	/// @return ItemType smart pointer or nulllptr on error
	///
	template<class Rep, class Period, typename ...Args>
	[[nodiscard]] auto
	try_alloc_for(const std::chrono::duration<Rep, Period>& timeout,
							Args&& ...args) noexcept
	{
		using namespace std::chrono;

		return alloc_impl(duration_cast<milliseconds>(timeout).count(),
						std::forward<Args>(args)...);
	}

	///
	/// @brief get the free block count
	///
	/// @return number of free blocks
	///
	auto free_block_count() noexcept
	{
		return k_mem_slab_num_free_get(&m_slab);
	}

	///
	/// @brief get the used block count
	///
	/// @return number of used blocks
	///
	auto used_block_count() noexcept
	{
		return k_mem_slab_num_used_get(&m_slab);
	}

	///
	/// @brief get the total number of blocks
	///
	/// @return total number of blocks
	///
	constexpr auto total_block_count() noexcept
	{
		return ItemCount;
	}
private:
	template<typename ...Args>
	auto alloc_impl(s32_t timeout, Args&& ...args) noexcept
	{
		void* res{nullptr};
		auto rc = k_mem_slab_alloc(&m_slab, &res, timeout);

		if (rc == 0) {
			auto p = new(res) ItemType{std::forward<Args>(args)...};

			return unique_ptr(p, mem_slab_deleter{this});
		} else {
			return unique_ptr(nullptr, mem_slab_deleter{this});
		}
	}
private:
	alignas(ItemAlignment) char m_slab_buffer[ItemSize * ItemCount];
	struct k_mem_slab m_slab{};
};

} // namespace zpp

#endif // ZPP__INCLUDE__ZPP__MEM_SLAB_HPP
