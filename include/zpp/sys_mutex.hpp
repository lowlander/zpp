///
/// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_SYS_MUTEX_HPP
#define ZPP_INCLUDE_ZPP_SYS_MUTEX_HPP

#ifdef CONFIG_USERSPACE

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <chrono>

namespace zpp {

///
/// @brief A userspace mutex class.
///
template<typename T_Mutex>
class sys_mutex_base
{
public:
  using native_type = struct sys_mutex;
  using native_pointer = native_type*;
  using native_cont_pointer = native_type const *;
protected:
  ///
  /// @brief Protected default contructor so only derived objects can be created
  ///
  constexpr sys_mutex_base() noexcept = default;

public:
  ///
  /// @brief Lock the mutex. Wait for ever until it is locked.
  ///
  /// @return true if successfully locked.
  ///
  [[nodiscard]] bool lock() noexcept
  {
    if (sys_mutex_lock(native_handle(), K_FOREVER) == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  /// @return true if successfully locked.
  ///
  [[nodiscard]] bool try_lock() noexcept
  {
    if (sys_mutex_lock(native_handle(), K_NO_WAIT) == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Try locking the mutex with a timeout.
  ///
  /// @param timeout The time to wait before returning
  ///
  /// @return true if successfully locked.
  ///
  template<class T_Rep, class T_Period>
  [[nodiscard]] bool
  try_lock_for(const std::chrono::duration<T_Rep, T_Period>& timeout) noexcept
  {
    using namespace std::chrono;

    if (sys_mutex_lock(native_handle(), to_timeout(timeout)) == 0)
    {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Unlock the mutex.
  ///
  [[nodiscard]] bool unlock() noexcept
  {
    if (sys_mutex_unlock(native_handle()) == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr sys_mutex.
  ///
  auto native_handle() noexcept -> native_pointer
  {
    return static_cast<T_Mutex*>(this)->native_handle();
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr sys_mutex.
  ///
  auto native_handle() const noexcept -> native_const_pointer
  {
    return static_cast<const T_Mutex*>(this)->native_handle();
  }
public:
  sys_mutex_base(const sys_mutex_base&) = delete;
  sys_mutex_base(sys_mutex_base&&) = delete;
  sys_mutex_base& operator=(const sys_mutex_base&) = delete;
  sys_mutex_base& operator=(sys_mutex_base&&) = delete;
};

///
/// @brief A recursive mutex class.
///
class sys_mutex : public sys_mutex_base<sys_mutex> {
public:
  ///
  /// @brief Default constructor
  ///
  sys_mutex() noexcept
  {
    sys_mutex_init(&m_mutex);
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr sys_mutex.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return &m_mutex;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr sys_mutex.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return &m_mutex;
  }
private:
  native_type m_mutex;
public:
  sys_mutex(const sys_mutex&) = delete;
  sys_mutex(sys_mutex&&) = delete;
  sys_mutex& operator=(const sys_mutex&) = delete;
  sys_mutex& operator=(sys_mutex&&) = delete;
};

///
/// @brief A recursive mutex class borrowing the native mutex.
///
class sys_mutex_ref : public sys_mutex_base<sys_mutex_ref> {
public:
  ///
  /// @brief Construct a mutex using a native sys_mutex*
  ///
  /// @param m The sys_mutex to use. @a m must already be
  ///          initialized and will not be freed.
  ///
  explicit constexpr sys_mutex_ref(native_pointer m) noexcept
    : m_mutex_ptr(m)
  {
    __ASSERT_NO_MSG(m_mutex_ptr != nullptr);
  }

  ///
  /// @brief Construct a mutex using a native sys_mutex*
  ///
  /// @param m The sys_mutex to use. @a m must already be
  ///          initialized and will not be freed.
  ///
  template<class T_Mutex>
  explicit constexpr sys_mutex_ref(T_Mutex& m) noexcept
    : m_mutex_ptr(m.native_handle())
  {
    __ASSERT_NO_MSG(m_mutex_ptr != nullptr);
  }

  ///
  /// @brief Construct a mutex using a native sys_mutex*
  ///
  /// @param m The sys_mutex to use. @a m must already be
  ///          initialized and will not be freed.
  ///
  constexpr sys_mutex_ref& operator=(native_pointer m) noexcept
  {
    m_mutex_ptr = m;
    __ASSERT_NO_MSG(m_mutex_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief Construct a mutex using a native sys_mutex*
  ///
  /// @param m The sys_mutex to use. @a m must already be
  ///          initialized and will not be freed.
  ///
  template<class T_Mutex>
  constexpr sys_mutex_ref& operator=(T_Mutex& m) noexcept
  {
    m_mutex_ptr = m.native_handle();
    __ASSERT_NO_MSG(m_mutex_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr sys_mutex.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return m_mutex_ptr;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr sys_mutex.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return m_mutex_ptr;
  }
private:
  native_pointer m_mutex_ptr{ nullptr };
public:
  sys_mutex_ref() = delete;
};

} // namespace zpp

#endif // CONFIG_USERSPACE

#endif // ZPP_INCLUDE_ZPP_SYS_MUTEX_HPP
