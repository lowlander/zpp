//
// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_HEAP_HPP
#define ZPP_INCLUDE_ZPP_HEAP_HPP

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <array>
#include <cstdint>

namespace zpp {

///
/// @brief Heap memory allocater CRTP base class
///
template <class T_Heap>
class base_heap {
public:
  using native_type = struct k_heap;
  using native_pointer = native_type*;
  using native_const_pointer = native_type const *;
protected:
  ///
  /// @brief default protected constructor so only derived objects can be created
  ///
  constexpr base_heap() noexcept { }
public:
  ///
  /// @brief Allocate memory from this heap wainting forever
  ///
  /// @param bytes the number of bytes to allocate
  ///
  /// @return The memory or nullptr on failure
  ///
  [[nodiscard]] void*
  allocate(size_t bytes) noexcept
  {
    return k_heap_alloc(native_handle(), bytes, K_FOREVER);
  }

  ///
  /// @brief Allocate memory from this heap waiting forever
  ///
  /// @param bytes the number of bytes to allocate
  /// @param align the alignment of the allocated memory
  ///
  /// @return The memory or nullptr on failure
  ///
  [[nodiscard]] void*
  allocate(size_t bytes, size_t align) noexcept
  {
    return k_heap_aligned_alloc(native_handle(), align, bytes, K_FOREVER);
  }

  ///
  /// @brief Allocate memory from this heap without waiting
  ///
  /// @param bytes the number of bytes to allocate
  ///
  /// @return The memory or nullptr on failure
  ///
  [[nodiscard]] void*
  try_allocate(size_t bytes) noexcept
  {
    return k_heap_alloc(native_handle(), bytes, K_NO_WAIT);
  }

  ///
  /// @brief Allocate memory from this heap without waiting
  ///
  /// @param bytes the number of bytes to allocate
  /// @param align the alignment of the allocated memory
  ///
  /// @return The memory or nullptr on failure
  ///
  [[nodiscard]] void*
  try_allocate(size_t bytes, size_t align) noexcept
  {
    return k_heap_aligned_alloc(native_handle(), align, bytes, K_NO_WAIT);
  }

  ///
  /// @brief Allocate memory from this heap waiting with a timeout
  ///
  /// @param bytes the number of bytes to allocate
  /// @param timeout the time to try the allocation
  ///
  /// @return The memory or nullptr on failure
  ///
  template<class T_Rep, class T_Period>
  [[nodiscard]] void*
  try_allocate_for(size_t bytes, const std::chrono::duration<T_Rep, T_Period>& timeout) noexcept
  {
    return k_heap_alloc(native_handle(), bytes, to_timeout(timeout));
  }

  ///
  /// @brief Allocate memory from this heap waiting with a timeout
  ///
  /// @param bytes the number of bytes to allocate
  /// @param align the alignment of the allocated memory
  /// @param timeout the time to try the allocation
  ///
  /// @return The memory or nullptr on failure
  ///
  template<class T_Rep, class T_Period>
  [[nodiscard]] void*
  try_allocate_for(size_t bytes, size_t align, const std::chrono::duration<T_Rep, T_Period>& timeout) noexcept
  {
    return k_heap_aligned_alloc(native_handle(), align, bytes, to_timeout(timeout));
  }

  ///
  /// @brief Deallocate memory previously allocated
  ///
  /// @param mem the memory to deallocate
  ///
  void deallocate(void* mem) noexcept
  {
    k_heap_free(native_handle(), mem);
  }

  ///
  /// @brief get the native zephyr heap handle.
  ///
  /// @return A pointer to the zephyr k_heap.
  ///
  auto native_handle() noexcept -> native_pointer
  {
    return static_cast<T_Heap*>(this)->native_handle();
  }

  ///
  /// @brief get the native zephyr heap handle.
  ///
  /// @return A pointer to the zephyr k_heap.
  ///
  auto native_handle() const noexcept -> native_const_pointer
  {
    return static_cast<const T_Heap*>(this)->native_handle();
  }
public:
  base_heap(const base_heap&) = delete;
  base_heap(base_heap&&) = delete;
  base_heap& operator=(const base_heap&) = delete;
  base_heap& operator=(base_heap&&) = delete;
};

///
/// @brief heap class
///
template <size_t T_Size>
class heap : public base_heap<heap<T_Size>> {
public:
  using typename base_heap<heap<T_Size>>::native_type;
  using typename base_heap<heap<T_Size>>::native_pointer;
  using typename base_heap<heap<T_Size>>::native_const_pointer;
public:
  ///
  /// @brief The default constructor
  ///
  heap() noexcept {
    k_heap_init(&m_heap, m_mem.data(), m_mem.size());
  }

  ///
  /// @brief Return the total size of this heap
  ///
  /// @return The heap size in bytes
  ///
  static constexpr size_t size() noexcept {
    return T_Size;
  }

  ///
  /// @brief get the native zephyr heap handle.
  ///
  /// @return A pointer to the zephyr k_heap.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return &m_heap;
  }

  ///
  /// @brief get the native zephyr heap handle.
  ///
  /// @return A pointer to the zephyr k_heap.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return &m_heap;
  }
private:
  native_type                 m_heap{};
  std::array<uint8_t, T_Size> m_mem;
public:
  heap(const heap&) = delete;
  heap(heap&&) = delete;
  heap& operator=(const heap&) = delete;
  heap& operator=(heap&&) = delete;
};

///
/// @brief heap reference class
///
/// @warning the referenced object must outlife the reference object
///
class heap_ref : public base_heap<heap_ref> {
public:
  ///
  /// @brief reference native heap object
  ///
  /// @param h the native heap object
  ///
  /// @warning The native heap object @a h must be valid for the lifetime
  ///          of this object
  ///
  constexpr explicit heap_ref(native_pointer h) noexcept
    : m_heap(h)
  {
    __ASSERT_NO_MSG(m_heap != nullptr);
  }

  ///
  /// @brief reference heap object
  ///
  /// @param h the heap object
  ///
  /// @warning The heap object @a h must be valid for the lifetime
  ///          of this object
  ///
  template<class T_Heap>
  constexpr explicit heap_ref(T_Heap& h) noexcept
    : m_heap(h.native_handle())
  {
    __ASSERT_NO_MSG(m_heap != nullptr);
  }

  ///
  /// @brief assign new native heap object
  ///
  /// @param h the native heap object
  ///
  /// @return reference to this object
  ///
  /// @warning The native heap object @a h must be valid for the lifetime
  ///          of this object
  ///
  constexpr heap_ref& operator=(native_pointer h) noexcept
  {
    m_heap = h;
    __ASSERT_NO_MSG(m_heap != nullptr);
    return *this;
  }

  ///
  /// @brief assign new heap object
  ///
  /// @param h the heap object
  ///
  /// @return reference to this object
  ///
  /// @warning The heap object @a h must be valid for the lifetime
  ///          of this object
  ///
  template<class T_Heap>
  constexpr heap_ref& operator=(T_Heap& rhs) noexcept
  {
    m_heap = rhs.native_handle();
    __ASSERT_NO_MSG(m_heap != nullptr);
    return *this;
  }

  ///
  /// @brief get the native zephyr heap handle.
  ///
  /// @return A pointer to the zephyr k_heap.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return m_heap;
  }

  ///
  /// @brief get the native zephyr heap handle.
  ///
  /// @return A pointer to the zephyr k_heap.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return m_heap;
  }
private:
  native_pointer m_heap{nullptr};
public:
  heap_ref() = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_HEAP_HPP
