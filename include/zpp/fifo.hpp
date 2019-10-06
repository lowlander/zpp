//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP__INCLUDE__ZPP__FIFO_HPP
#define ZPP__INCLUDE__ZPP__FIFO_HPP

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <limits>
#include <type_traits>
#include <cstddef>

namespace zpp {

///
/// @brief obligated base class to use for fifo items
///
struct fifo_item_base {
	void* reserved { nullptr };
};

///
/// @brief fifo base class
///
/// @param FifoType the CRTP derived type
/// @param ItemType the item to store in this fifo
///
template<typename FifoType, typename ItemType>
class fifo_base {
protected:
	///
	/// @brief default constructor, can only be called from derived types
	///
	fifo_base() noexcept
	{
		static_assert(std::is_standard_layout<fifo_item_base>::value);
		static_assert(std::is_pointer<decltype(fifo_item_base::reserved)>::value);
		static_assert(offsetof(fifo_item_base, reserved) == 0);
		static_assert(std::is_base_of<fifo_item_base, ItemType>::value);
	}

public:
	///
	/// @brief get the Zephyr native fifo handle
	///
	/// @return pointer to a k_fifo
	///
	[[nodiscard]] auto native_handle() noexcept
	{
		return static_cast<FifoType*>(this)->native_handle();
	}

	///
	/// @brief force a waiting thread to return with a timeout error
	///
	void cancel_wait() noexcept
	{
		k_fifo_cancel_wait(native_handle());
	}

	///
	/// @brief push an item on the back of the fifo
	///
	/// @param item Pointer to a item, the fifo does not take ownership
	///
	void push_back(ItemType* item) noexcept
	{
		k_fifo_put(native_handle(), item);
	}

	///
	/// @brief pop item from fifo waiting for ever
	///
	/// @return the item or nullptr on error
	///
	[[nodiscard]] ItemType*
	pop_front() noexcept
	{
		return static_cast<ItemType*>(
			k_fifo_get(native_handle(), K_FOREVER));
	}

	///
	/// @brief try to pop item from the fifo without waiting
	///
	/// @return the item or nullptr on error/timeout
	///
	[[nodiscard]] ItemType*
	try_pop_front() noexcept
	{
		return static_cast<ItemType*>(
			k_fifo_get(native_handle(), K_NO_WAIT));
	}

	///
	/// @brief try to pop item from the fifo waiting a certain amount of time
	///
	/// @return the item or nullptr on error/timeout
	///
	template <class Rep, class Period>
	[[nodiscard]] ItemType*
	try_pop_front_for(const std::chrono::duration<Rep, Period>& timeout) noexcept
	{
		using namespace std::chrono;

		return static_cast<ItemType*>(
			k_fifo_get(native_handle(),
			duration_cast<milliseconds>(timeout).count()));
	}

	///
	/// @brief get item at the front without removing it from the fifo
	///
	/// @return the item or nullptr on error/timeout
	///
	[[nodiscard]] ItemType*
	front() noexcept
	{
		return static_cast<ItemType*>(
			k_fifo_peek_head(native_handle()));
	}

	///
	/// @brief get item at the back without removing it from the fifo
	///
	/// @return the item or nullptr on error/timeout
	///
	[[nodiscard]] ItemType* back() noexcept
	{
		return static_cast<ItemType*>(
			k_fifo_peek_tail(native_handle()));
	}

	///
	/// @brief check if the fifo is empty
	///
	/// @return true if the fifo is empty
	///
	[[nodiscard]] bool empty() noexcept
	{
		auto res  = k_fifo_is_empty(native_handle());
		if (res == 0) {
			return false;
		} else {
			return true;
		}
	}
public:
	fifo_base(const fifo_base&) = delete;
	fifo_base(fifo_base&&) = delete;
	fifo_base& operator=(const fifo_base&) = delete;
	fifo_base& operator=(fifo_base&&) = delete;
};

///
/// @brief fifo that manages a k_fifo object
///
template<typename ItemType>
class fifo : public fifo_base<fifo<ItemType>, ItemType> {
public:
	///
	/// @brief create new fifo
	///
	fifo() noexcept
	{
		k_fifo_init(&m_fifo);
	}

	///
	/// @brief get the Zephyr native fifo handle
	///
	/// @return pointer to a k_fifo
	///
	[[nodiscard]] auto native_handle() noexcept
	{
		return &m_fifo;
	}
private:
	struct k_fifo m_fifo;
public:
	fifo(const fifo&) = delete;
	fifo(fifo&&) = delete;
	fifo& operator=(const fifo&) = delete;
	fifo& operator=(fifo&&) = delete;
};

///
/// @brief fifo that borrowes a k_fifo object
///
template<typename ItemType>
class borrowed_fifo : public fifo_base<fifo<ItemType>, ItemType> {
public:
	///
	/// @brief wrap k_fifo
	///
	/// @param f the k_fifo to borrow
	///
	borrowed_fifo(struct k_fifo* f) noexcept
		: m_fifo_ptr(f)
	{
	}

	///
	/// @brief get the Zephyr native fifo handle
	///
	/// @return pointer to a k_fifo
	///
	[[nodiscard]] auto native_handle() noexcept
	{
		return m_fifo_ptr;
	}
private:
	struct k_fifo* m_fifo_ptr{ nullptr };
public:
	borrowed_fifo(const borrowed_fifo&) = delete;
	borrowed_fifo(borrowed_fifo&&) = delete;
	borrowed_fifo& operator=(const borrowed_fifo&) = delete;
	borrowed_fifo& operator=(borrowed_fifo&&) = delete;
};

} // namespace zpp

#endif // ZPP__INCLUDE__ZPP__FIFO_HPP
