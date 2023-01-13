///
/// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_MUTEX_HPP
#define ZPP_INCLUDE_ZPP_MUTEX_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <chrono>

#include <zpp/result.hpp>
#include <zpp/error_code.hpp>

namespace zpp {

///
/// @brief A recursive mutex CRTP base class.
///
template<typename T_Mutex>
class mutex_base
{
public:
  using native_type = struct k_mutex;
  using native_pointer = native_type*;
  using native_const_pointer = native_type const *;
protected:
  ///
  /// @brief Protected default constructor so only derived objects can be created
  ///
  constexpr mutex_base() noexcept
  {
  }

public:
  ///
  /// @brief Lock the mutex. Wait forever until it is locked.
  ///
  /// @return true if successfully locked.
  ///
  [[nodiscard]] auto lock() noexcept
  {
    result<void, error_code> res;

    auto rc = k_mutex_lock(native_handle(), K_FOREVER);
    if (rc == 0) {
      res.assign_value();
    } else {
      res.assign_error(to_error_code(-rc));
    }

    return res;
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  /// @return true if successfully locked.
  ///
  [[nodiscard]] auto try_lock() noexcept
  {
    result<void, error_code> res;

    auto rc = k_mutex_lock(native_handle(), K_NO_WAIT);
    if (rc == 0) {
      res.assign_value();
    } else {
      res.assign_error(to_error_code(-rc));
    }

    return res;
  }

  ///
  /// @brief Try locking the mutex with a timeout.
  ///
  /// @param timeout The time to wait before returning
  ///
  /// @return true if successfully locked.
  ///
  template<class T_Rep, class T_Period>
  [[nodiscard]] auto
  try_lock_for(const std::chrono::duration<T_Rep, T_Period>& timeout) noexcept
  {
    using namespace std::chrono;

    result<void, error_code> res;

    auto rc = k_mutex_lock(native_handle(), to_timeout(timeout));
    if (rc == 0) {
      res.assign_value();
    } else {
      res.assign_error(to_error_code(-rc));
    }

    return res;
  }

  ///
  /// @brief Unlock the mutex.
  ///
  /// @return true on success
  ///
  [[nodiscard]] auto unlock() noexcept
  {
    result<void, error_code> res;

    auto rc = k_mutex_unlock(native_handle());
    if (rc == 0) {
      res.assign_value();
    } else {
      res.assign_error(to_error_code(-rc));
    }

    return res;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  auto native_handle() noexcept -> native_pointer
  {
    return static_cast<T_Mutex*>(this)->native_handle();
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  auto native_handle() const noexcept -> native_const_pointer
  {
    return static_cast<const T_Mutex*>(this)->native_handle();
  }
public:
  mutex_base(const mutex_base&) = delete;
  mutex_base(mutex_base&&) = delete;
  mutex_base& operator=(const mutex_base&) = delete;
  mutex_base& operator=(mutex_base&&) = delete;
};

///
/// @brief A recursive mutex class.
///
class mutex : public mutex_base<mutex> {
public:
  ///
  /// @brief Default contructor
  ///
  mutex() noexcept
  {
    k_mutex_init(&m_mutex);
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return &m_mutex;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return &m_mutex;
  }
private:
  native_type m_mutex{};
public:
  mutex(const mutex&) = delete;
  mutex(mutex&&) = delete;
  mutex& operator=(const mutex&) = delete;
  mutex& operator=(mutex&&) = delete;
};

///
/// @brief A recursive mutex class borrowing the native mutex.
///
class mutex_ref : public mutex_base<mutex_ref> {
public:
  ///
  /// @brief Construct a mutex using a native k_mutex*
  ///
  /// @param m The k_mutex to use. @a m must already be
  ///          initialized and will not be freed.
  ///
  explicit constexpr mutex_ref(native_pointer m) noexcept
    : m_mutex_ptr(m)
  {
    __ASSERT_NO_MSG(m_mutex_ptr != nullptr);
  }

  ///
  /// @brief Construct a mutex using another mutex object
  ///
  /// @param m The mutex object to use. @a m must already be
  ///          initialized and will not be freed.
  ///
  template<class T_Mutex>
  explicit constexpr mutex_ref(T_Mutex& m) noexcept
    : m_mutex_ptr(m.native_handle())
  {
    __ASSERT_NO_MSG(m_mutex_ptr != nullptr);
  }

  ///
  /// @brief Assing another mutex object
  ///
  /// @param m The k_mutex to use. @a m must already be
  ///          initialized and will not be freed.
  ///
  /// @return reference to this object
  ///
  constexpr mutex_ref& operator=(native_pointer m) noexcept
  {
    m_mutex_ptr = m;
    __ASSERT_NO_MSG(m_mutex_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief Assing another mutex object
  ///
  /// @param m The mutex object to use. @a m must already be
  ///          initialized and will not be freed.
  ///
  /// @return reference to this object
  ///
  template<class T_Mutex>
  constexpr mutex_ref& operator=(T_Mutex& m) noexcept
  {
    m_mutex_ptr = m.native_handle();
    __ASSERT_NO_MSG(m_mutex_ptr != nullptr);
    return *this;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    return m_mutex_ptr;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    return m_mutex_ptr;
  }
private:
  native_pointer m_mutex_ptr{ nullptr };
public:
  mutex_ref() = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_MUTEX_HPP
