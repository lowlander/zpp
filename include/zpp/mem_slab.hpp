//
// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_MEM_SLAB_HPP
#define ZPP_INCLUDE_ZPP_MEM_SLAB_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <cstdint>
#include <chrono>
#include <array>

#include <zpp/utils.hpp>

namespace zpp {

///
/// @brief Allocator that uses k_mem_slab for memory
///
/// @param T_MemSlab the CRTP type
///
template<class T_MemSlab>
class mem_slab_base {
public:
  using native_type = struct k_mem_slab;
  using native_pointer = native_type*;
  using native_const_pointer = native_type const *;
protected:
  constexpr mem_slab_base() noexcept {}
public:
  ///
  /// @brief allocate a memory block, waiting forever
  ///
  /// @return pointer to memory or nullptr on error
  ///
  [[nodiscard]] void*
  allocate() noexcept
  {
    void* vp{nullptr};

    auto rc = k_mem_slab_alloc(native_handle(), &vp, K_FOREVER);

    if (rc == 0) {
      return vp;
    } else {
      return nullptr;
    }
  }

  ///
  /// @brief try allocate a memory block, not waiting
  ///
  /// @return pointer to memory or nullptr on error
  ///
  [[nodiscard]] void*
  try_allocate() noexcept
  {
    void* vp{nullptr};

    auto rc = k_mem_slab_alloc(native_handle(), &vp, K_NO_WAIT);

    if (rc == 0) {
      return vp;
    } else {
      return nullptr;
    }
  }

  ///
  /// @brief try allocate a memory block waiting with a timeout
  ///
  /// @param timeout the time to try
  ///
  /// @return pointer to memory or nullptr on error
  ///
  template<class T_Rep, class T_Period>
  [[nodiscard]] void*
  try_allocate_for(const std::chrono::duration<T_Rep, T_Period>& timeout) noexcept
  {
    using namespace std::chrono;

    void* vp{nullptr};

    auto rc = k_mem_slab_alloc(native_handle(), &vp, to_timeout(timeout));

    if (rc == 0) {
      return vp;
    } else {
      return nullptr;
    }
  }

  ///
  /// @brief deallocate memory
  ///
  /// @param vp the pointer to free
  ///
  void deallocate(void* vp) noexcept
  {
    if (vp != nullptr) {
      k_mem_slab_free(native_handle(), &vp);
    }
  }

  ///
  /// @brief the size of the memory blocks
  ///
  /// @return the size of the memory blocks in bytes
  ///
  constexpr auto block_size() const noexcept
  {
    return native_handle()->block_size;
  }

  ///
  /// @brief get maximm number of blocks that can be allocated
  ///
  /// @return the maximum number of blocks that can be allocated
  ///
  constexpr auto total_block_count() const noexcept
  {
    return native_handle()->num_blocks;
  }

  ///
  /// @brief get current number of used blocks
  ///
  /// @return the current number of used blocks
  ///
  constexpr auto used_block_count() noexcept
  {
    return k_mem_slab_num_used_get(native_handle());
  }

  ///
  /// @brief get current number of free blocks
  ///
  /// @return the current number of free blocks
  ///
  constexpr auto free_block_count() noexcept
  {
    return k_mem_slab_num_free_get(native_handle());
  }


  ///
  /// @brief get the native zephyr mem slab handle.
  ///
  /// @return A pointer to the zephyr k_mem_slab.
  ///
  auto native_handle() noexcept -> native_pointer
  {
    return static_cast<T_MemSlab*>(this)->native_handle();
  }

  ///
  /// @brief get the native zephyr mem slab handle.
  ///
  /// @return A pointer to the zephyr k_mem_slab.
  ///
  auto native_handle() const noexcept -> native_const_pointer
  {
    return static_cast<const T_MemSlab*>(this)->native_handle();
  }
public:
  mem_slab_base(const mem_slab_base&) = delete;
  mem_slab_base(mem_slab_base&&) = delete;
  mem_slab_base& operator=(const mem_slab_base&) = delete;
  mem_slab_base& operator=(mem_slab_base&&) = delete;
};


///
/// @brief A memory slab class.
///
template<uint32_t T_BlockSize, uint32_t T_BlockCount, uint32_t T_Align=sizeof(void*)>
class mem_slab : public mem_slab_base<mem_slab<T_BlockSize, T_BlockCount, T_Align>>
{
  static_assert(T_BlockCount > 0);
  static_assert(is_multiple_of(T_BlockSize, 4) == true);
  static_assert(T_Align >= sizeof(void*));
  static_assert(is_power_of_two(T_Align));
  static_assert(T_BlockSize >= T_Align);
  static_assert((T_BlockSize % T_Align) == 0);
public:
  using typename mem_slab_base<mem_slab<T_BlockSize, T_BlockCount, T_Align>>::native_type;
  using typename mem_slab_base<mem_slab<T_BlockSize, T_BlockCount, T_Align>>::native_pointer;
  using typename mem_slab_base<mem_slab<T_BlockSize, T_BlockCount, T_Align>>::native_const_pointer;
public:
  ///
  /// @brief Default constructor
  ///
  mem_slab() noexcept
  {
    k_mem_slab_init(&m_mem_slab, m_mem_buffer.data(), T_BlockSize, T_BlockCount);
  }

  ///
  /// @brief get the native zephyr mem slab handle.
  ///
  /// @return A pointer to the zephyr k_mem_slab.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return &m_mem_slab;
  }

  ///
  /// @brief get the native zephyr mem slab handle.
  ///
  /// @return A pointer to the zephyr k_mem_slab.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return &m_mem_slab;
  }
private:
  native_type                                                       m_mem_slab{};
  alignas(T_Align) std::array<uint8_t, T_BlockSize * T_BlockCount>  m_mem_buffer;
public:
  mem_slab(const mem_slab&) = delete;
  mem_slab(mem_slab&&) = delete;
  mem_slab& operator=(const mem_slab&) = delete;
  mem_slab& operator=(mem_slab&&) = delete;
};

///
/// @brief A mem_slab class referencing another mem slab object.
///
class mem_slab_ref : public mem_slab_base<mem_slab_ref> {
public:
  ///
  /// @brief Construct a reference to a native k_mem_slab*
  ///
  /// @param m The k_mem_slab to reference. @a m must already be
  ///          initialized and will not be freed.
  ///
  explicit constexpr mem_slab_ref(native_pointer m) noexcept
    : m_mem_slab_ptr(m)
  {
    __ASSERT_NO_MSG(m_mem_slab_ptr != nullptr);
  }

  ///
  /// @brief Construct a reference to another mem_slab object
  ///
  /// @param m The object to reference. @a m must already be
  ///          initialized and will not be freed.
  ///
  template<class T_MemSlab>
  explicit constexpr mem_slab_ref(T_MemSlab& m) noexcept
    : m_mem_slab_ptr(m.native_handle())
  {
    __ASSERT_NO_MSG(m_mem_slab_ptr != nullptr);
  }

  ///
  /// @brief Assign a new native k_mem_slab* object
  ///
  /// @param m The k_mem_slab to reference. @a m must already be
  ///          initialized and will not be freed.
  ///
  /// @return reference to this object
  ///
  constexpr mem_slab_ref& operator=(native_pointer m) noexcept
  {
    m_mem_slab_ptr = m;
    __ASSERT_NO_MSG(m_mem_slab_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief Assign a new native mem slab object
  ///
  /// @param m The object to reference. @a m must already be
  ///          initialized and will not be freed.
  ///
  /// @return reference to this object
  ///
  template<class T_MemSlab>
  constexpr mem_slab_ref& operator=(T_MemSlab& m) noexcept
  {
    m_mem_slab_ptr = m.native_handle();
    __ASSERT_NO_MSG(m_mem_slab_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief get the native zephyr mem slab handle.
  ///
  /// @return A pointer to the zephyr k_mem_slab.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return m_mem_slab_ptr;
  }

  ///
  /// @brief get the native zephyr mem slab handle.
  ///
  /// @return A pointer to the zephyr k_mem_slab.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return m_mem_slab_ptr;
  }
private:
  native_pointer m_mem_slab_ptr{ nullptr };
public:
  mem_slab_ref() = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_MEM_SLAB_HPP
