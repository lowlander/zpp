//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_SCHED_HPP
#define ZPP_INCLUDE_ZPP_SCHED_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

namespace zpp {

///
/// @brief Lock the scheduler.
///
inline void sched_lock() noexcept
{
  k_sched_lock();
}

///
/// @brief Unlock the scheduler.
///
inline void sched_unlock() noexcept
{
  k_sched_unlock();
}

///
/// @brief Guard to automatically lock/unlock scheduler.
///
class sched_lock_guard {
public:
  ///
  /// @brief Default constructor that calls scheck_lock()
  ///
  sched_lock_guard() noexcept
  {
    sched_lock();
  }

  ///
  /// @brief Destructor that calls sched_unlock()
  ///
  ~sched_lock_guard()
  {
    sched_unlock();
  }
public:
  sched_lock_guard(const sched_lock_guard&) = delete;
  sched_lock_guard& operator=(const sched_lock_guard&) = delete;
  sched_lock_guard(sched_lock_guard&&) noexcept = delete;
  sched_lock_guard& operator=(sched_lock_guard&&) noexcept = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_SCHED_HPP
