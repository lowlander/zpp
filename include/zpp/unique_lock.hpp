///
/// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_UNIQUE_LOCK_HPP
#define ZPP_INCLUDE_ZPP_UNIQUE_LOCK_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <chrono>

#include <zpp/result.hpp>
#include <zpp/error_code.hpp>

namespace zpp {

///
/// @brief zpp::unique_lock using zpp::mutex as a lock.
///
template<typename T_Mutex>
class unique_lock {
public:
  using native_pointer = T_Mutex::native_pointer;
  using native_const_pointer = T_Mutex::native_const_pointer;
public:
  ///
  /// @brief Try locking the mutex without waiting.
  ///
  unique_lock() noexcept = default;

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  explicit unique_lock(T_Mutex& lock) noexcept
    : m_lock(&lock)
  {
    __ASSERT_NO_MSG(m_lock != nullptr);
    auto res = m_lock->lock();
    __ASSERT_NO_MSG(res != false);
    m_is_owner = true;
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  unique_lock(unique_lock&& src) noexcept
    : m_lock(src.m_lock)
    , m_is_owner(src.m_is_owner)
  {
    src.m_lock = nullptr;
    src.m_is_owner = false;
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  unique_lock& operator=(unique_lock&& src) noexcept
  {
    if (m_lock != nullptr && m_is_owner) {
      auto res = m_lock->unlock();
      __ASSERT_NO_MSG(res != false);
    }

    m_lock = src.m_lock;
    m_is_owner = src.m_is_owner;

    src.m_lock = nullptr;
    src.m_is_owner = false;

    return *this;
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  ~unique_lock() noexcept
  {
    if (m_lock != nullptr && m_is_owner) {
      auto res = m_lock->unlock();
      __ASSERT_NO_MSG(res != false);
    }
  }

  ///
  /// @brief Lock the mutex. Wait for ever until it is locked.
  ///
  /// @return true if successfully locked.
  ///
  [[nodiscard]] auto lock() noexcept
  {
    result<void, error_code> res;

    if (m_lock == nullptr) {
      res.assign_error(error_code::k_inval);
    } else if (m_is_owner == true) {
      res.assign_error(error_code::k_deadlk);
    } else {
      res = m_lock->lock();
      m_is_owner = (bool)res;
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

    if (m_lock == nullptr) {
      res.assign_error(error_code::k_inval);
    } else if (m_is_owner == true) {
      res.assign_error(error_code::k_deadlk);
    } else {
      res = m_lock->try_lock();
      m_is_owner = (bool)res;
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
    result<void, error_code> res;

    if (m_lock == nullptr) {
      res.assign_error(error_code::k_inval);
    } else if (m_is_owner == true) {
      res.assign_error(error_code::k_deadlk);
    } else {
      res = m_lock->try_lock_for(timeout);
      m_is_owner = (bool)res;
    }

    return res;
  }

  ///
  /// @brief Unlock the mutex.
  ///
  [[nodiscard]] auto unlock() noexcept
  {
    result<void, error_code> res;

    if (m_is_owner == false) {
      res.assign_error(error_code::k_perm);
    } else if (m_lock == nullptr) {
      res.assign_error(error_code::k_inval);
    } else {
      res = m_lock->unlock();
      m_is_owner = false;
    }

    return res;
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  constexpr T_Mutex* release() noexcept
  {
    auto ret = m_lock;
    m_lock = nullptr;
    m_is_owner = false;
    return ret;
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  constexpr bool owns_lock() const noexcept
  {
    return m_is_owner;
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  explicit constexpr operator bool() const noexcept
  {
    return owns_lock();
  }

  ///
  /// @brief Try locking the mutex without waiting.
  ///
  constexpr T_Mutex* mutex() const noexcept
  {
    return m_lock;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  constexpr auto native_handle() noexcept -> native_pointer
  {
    if (m_lock != nullptr)
      return m_lock->native_handle();
    else
      return nullptr;
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  constexpr auto native_handle() const noexcept -> native_const_pointer
  {
    if (m_lock != nullptr)
      return m_lock->native_handle();
    else
      return nullptr;
  }
private:
  T_Mutex*  m_lock{nullptr};
  bool      m_is_owner{false};
public:
  unique_lock(const unique_lock&) = delete;
  unique_lock& operator=(const unique_lock&) = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_UNIQUE_LOCK_HPP
