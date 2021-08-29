//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_FIFO_HPP
#define ZPP_INCLUDE_ZPP_FIFO_HPP

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <limits>
#include <type_traits>
#include <cstddef>

namespace zpp {

///
/// @brief Fifo CRTP base class
///
/// @param T_BaseFifoType the CRTP derived type
/// @param T_BaseItemType the item to store in this fifo
///
template<template<typename> typename T_BaseFifoType, typename T_BaseItemType>
class fifo_base {
public:
  using native_type = struct k_fifo;
  using native_pointer = native_type *;
  using native_const_pointer = native_type const *;

  using item_type = T_BaseItemType;
  using item_pointer = item_type*;
  using item_const_pointer = item_type const *;
protected:
  ///
  /// @brief default constructor, can only be called from derived types
  ///
  fifo_base() noexcept
  {
    static_assert(std::is_standard_layout_v<item_type>);
    static_assert(std::is_same_v<void*, decltype(item_type::fifo_reserved)>);
    static_assert(offsetof(item_type, fifo_reserved) == 0);
  }
public:
  ///
  /// @brief get the Zephyr native fifo handle
  ///
  /// @return pointer to a k_fifo
  ///
  [[nodiscard]] constexpr auto native_handle() noexcept -> native_pointer
  {
    return static_cast<T_BaseFifoType<item_type>*>(this)->native_handle();
  }

  ///
  /// @brief get the Zephyr native fifo handle
  ///
  /// @return pointer to a k_fifo
  ///
  [[nodiscard]] constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return static_cast<const T_BaseFifoType<item_type>*>(this)->native_handle();
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
  void push_back(item_pointer item) noexcept
  {
    k_fifo_put(native_handle(), item);
  }

  ///
  /// @brief pop item from fifo waiting for ever
  ///
  /// @return the item or nullptr on error
  ///
  [[nodiscard]] item_pointer
  pop_front() noexcept
  {
    return static_cast<item_pointer>(
      k_fifo_get(native_handle(), K_FOREVER));
  }

  ///
  /// @brief try to pop item from the fifo without waiting
  ///
  /// @return the item or nullptr on error/timeout
  ///
  [[nodiscard]] item_pointer
  try_pop_front() noexcept
  {
    return static_cast<item_pointer>(
      k_fifo_get(native_handle(), K_NO_WAIT));
  }

  ///
  /// @brief try to pop item from the fifo waiting a certain amount of time
  ///
  /// @param timeout The timeout before returning
  ///
  /// @return the item or nullptr on error/timeout
  ///
  template <class T_Rep, class T_Period>
  [[nodiscard]] item_pointer
  try_pop_front_for(const std::chrono::duration<T_Rep, T_Period>& timeout) noexcept
  {
    using namespace std::chrono;

    return static_cast<item_pointer>(
      k_fifo_get(native_handle(),
      duration_cast<milliseconds>(timeout).count()));
  }

  ///
  /// @brief get item at the front without removing it from the fifo
  ///
  /// @return the item or nullptr on error/timeout
  ///
  [[nodiscard]] item_pointer
  front() noexcept
  {
    return static_cast<item_pointer>(
      k_fifo_peek_head(native_handle()));
  }

  ///
  /// @brief get item at the back without removing it from the fifo
  ///
  /// @return the item or nullptr on error/timeout
  ///
  [[nodiscard]] item_pointer back() noexcept
  {
    return static_cast<item_pointer>(
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
template<typename T_ItemType>
class fifo : public fifo_base<fifo, T_ItemType> {
public:
  using typename fifo_base<fifo, T_ItemType>::native_type;
  using typename fifo_base<fifo, T_ItemType>::native_pointer;
  using typename fifo_base<fifo, T_ItemType>::native_const_pointer;
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
  [[nodiscard]] constexpr auto native_handle() noexcept -> native_pointer
  {
    return &m_fifo;
  }

  ///
  /// @brief get the Zephyr native fifo handle
  ///
  /// @return pointer to a k_fifo
  ///
  [[nodiscard]] constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return &m_fifo;
  }
private:
  native_type m_fifo;
public:
  fifo(const fifo&) = delete;
  fifo(fifo&&) = delete;
  fifo& operator=(const fifo&) = delete;
  fifo& operator=(fifo&&) = delete;
};

///
/// @brief fifo that references a k_fifo object
///
template<typename T_ItemType>
class fifo_ref : public fifo_base<fifo_ref, T_ItemType> {
public:
  using typename fifo_base<fifo_ref, T_ItemType>::native_type;
  using typename fifo_base<fifo_ref, T_ItemType>::native_pointer;
  using typename fifo_base<fifo_ref, T_ItemType>::native_const_pointer;
public:
  ///
  /// @brief wrap k_fifo
  ///
  /// @param f the k_fifo to reference
  ///
  /// @warning @a f must stay valid for the lifetime of this object
  ///
  constexpr explicit fifo_ref(native_pointer f) noexcept
    : m_fifo_ptr(f)
  {
    __ASSERT_NO_MSG(m_fifo_ptr != nullptr);
  }

  ///
  /// @brief Reference another fifo object
  ///
  /// @param f the object to reference
  ///
  /// @warning @a f must stay valid for the lifetime of this object
  ///
  template<template<class> class T_Fifo>
  constexpr explicit fifo_ref(T_Fifo<T_ItemType>& f) noexcept
    : m_fifo_ptr(f.native_handle())
  {
    __ASSERT_NO_MSG(m_fifo_ptr != nullptr);
  }

  ///
  /// @brief Reference another fifo object
  ///
  /// @param f the object to reference
  ///
  /// @return *this
  ///
  /// @warning @a f must stay valid for the lifetime of this object
  ///
  constexpr fifo_ref& operator=(native_pointer f) noexcept
  {
    m_fifo_ptr = f;
    __ASSERT_NO_MSG(m_fifo_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief Reference another fifo object
  ///
  /// @param f the object to reference
  ///
  /// @return *this
  ///
  /// @warning @a f must stay valid for the lifetime of this object
  ///
  template<template<class> class T_Fifo>
  constexpr fifo_ref& operator=(const T_Fifo<T_ItemType>& f) noexcept
  {
    m_fifo_ptr = f.native_handle();
    __ASSERT_NO_MSG(m_fifo_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief get the Zephyr native fifo handle
  ///
  /// @return pointer to a k_fifo
  ///
  [[nodiscard]] constexpr auto native_handle() noexcept -> native_pointer
  {
    return m_fifo_ptr;
  }

  ///
  /// @brief get the Zephyr native fifo handle
  ///
  /// @return pointer to a k_fifo
  ///
  [[nodiscard]] constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return m_fifo_ptr;
  }
private:
  native_pointer m_fifo_ptr{ nullptr };
public:
  fifo_ref() = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_FIFO_HPP
