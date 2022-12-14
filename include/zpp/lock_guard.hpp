///
/// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
///
/// SPDX-License-Identifier: Apache-2.0
///

#ifndef ZPP_INCLUDE_ZPP_LOCK_GUARD_HPP
#define ZPP_INCLUDE_ZPP_LOCK_GUARD_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <chrono>

namespace zpp {

///
/// @brief lock_guard using zpp::mutex as a lock.
///
/// @param T_Mutex the mutex type to use
///
template<typename T_Mutex>
class lock_guard {
public:
  ///
  /// @brief Try locking the mutex waiting forever.
  ///
  /// @param lock the mutex to lock
  ///
  explicit lock_guard(T_Mutex& lock) noexcept
    : m_lock(lock)
  {
    auto res = m_lock.lock();
    __ASSERT_NO_MSG(res != false);
  }

  ///
  /// @brief unlock the mutex.
  ///
  ~lock_guard()
  {
    auto res = m_lock.unlock();
    __ASSERT_NO_MSG(res != false);
  }
private:
  T_Mutex& m_lock;
public:
  lock_guard() = delete;
  lock_guard(lock_guard&&) = delete;
  lock_guard(const lock_guard&) = delete;
  lock_guard& operator=(lock_guard&&) = delete;
  lock_guard& operator=(const lock_guard&) = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_LOCK_GUARD_HPP
