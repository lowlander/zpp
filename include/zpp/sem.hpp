//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_SEM_HPP
#define ZPP_INCLUDE_ZPP_SEM_HPP

#include <zpp/thread.hpp>

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <limits>

namespace zpp {

///
/// @brief Counting semaphore base class
///
template<typename Sem>
class sem_base
{
public:
  ///
  /// @brief Type used as counter
  ///
  using counter_type = uint32_t;

  ///
  /// @brief Maxium value of the counter
  ///
  constexpr static counter_type max_count =
        std::numeric_limits<counter_type>::max();
protected:
  ///
  /// @brief Default constructor, only allowed derived objects
  ///
  constexpr sem_base() noexcept
  {
  }
public:
  ///
  /// @brief Take the semaphore waiting forever
  ///
  /// @return true when semaphore was taken
  ///
  [[nodiscard]] bool take() noexcept
  {
    if (k_sem_take(native_handle(), K_FOREVER) == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Try to take the semaphore without waiting
  ///
  /// @return true when semaphore was taken
  ///
  [[nodiscard]] bool try_take() noexcept
  {
    if (k_sem_take(native_handle(), K_NO_WAIT) == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Try to take the semaphore waiting a certain timeout
  ///
  /// @param timeout_duration The timeout to wait before giving up
  ///
  /// @return true when semaphore was taken
  ///
  template < class Rep, class Period>
  [[nodiscard]] bool
  try_take_for(const std::chrono::duration<Rep, Period>&
            timeout_duration) noexcept
  {
    if (k_sem_take(native_handle(), to_timeout(timeout_duration)) == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief Give the semaphore.
  ///
  void give() noexcept
  {
    k_sem_give(native_handle());
  }

  ///
  /// @brief Reset the semaphore counter to zero.
  ///
  void reset() noexcept
  {
    k_sem_reset(native_handle());
  }

  ///
  /// @brief Get current semaphore count
  ///
  /// @return The current semaphore count.
  ///
  counter_type count() noexcept
  {
    return k_sem_count_get(native_handle());
  }

  ///
  /// @brief Give the semaphore.
  ///
  void operator++(int) noexcept
  {
    give();
  }

  ///
  /// @brief Take the semaphore waiting forever
  ///
  void operator--(int) noexcept
  {
    while (!take()) {
      this_thread::yield();
    }
  }

  ///
  /// @brief Give the semaphore n times.
  ///
  /// @param n The number of times to give the semaphore
  ///
  void operator+=(int n) noexcept
  {
    while (n-- > 0) {
      give();
    }
  }

  ///
  /// @brief Take the semaphore n times, waiting forever.
  ///
  /// @param n The number of times to take the semaphore
  ///
  void operator-=(int n) noexcept
  {
    while (n-- > 0) {
      while (!take()) {
        this_thread::yield();
      }
    }
  }

  ///
   /// @brief get the native zephyr sem handle.
  ///
  /// @return A pointer to the zephyr k_sem.
   ///
  auto native_handle() noexcept
  {
    return static_cast<Sem*>(this)->native_handle();
  }
public:
  sem_base(const sem_base&) = delete;
  sem_base(sem_base&&) = delete;
  sem_base& operator=(const sem_base&) = delete;
  sem_base& operator=(sem_base&&) = delete;
};


///
/// @brief A counting semaphore class.
///
class sem : public sem_base<sem> {
public:
  ///
  /// @brief Constructor initializing initial count and count limit.
  ///
  /// @param initial_count The initial count value for the semaphore
  /// @param count_limit The maxium count the semaphore can have
  ///
  sem(counter_type initial_count, counter_type count_limit) noexcept
  {
    k_sem_init(&m_sem, initial_count, count_limit);
  }

  ///
  /// @brief Constructor initializing initial count.
  ///
  /// Contructor initializing initial count to @a initial_count and
  /// the maxium count limit to max_count.
  ///
  /// @param initial_count The initial count value for the semaphore
  ///
  explicit sem(counter_type initial_count) noexcept
    : sem(initial_count, max_count)
  {
  }

  ///
  /// @brief Default onstructor.
  ///
  /// Contructor initializing initial count to 0 and the maxium count
  /// limit to max_count.
  //
  sem() noexcept
    : sem(0, max_count)
  {
  }

  ///
  /// @brief get the native zephyr mutex handle.
  ///
  /// @return A pointer to the zephyr k_mutex.
  ///
  constexpr auto native_handle() noexcept
  {
    return &m_sem;
  }
private:
  struct k_sem m_sem;
public:
  sem(const sem&) = delete;
  sem(sem&&) = delete;
  sem& operator=(const sem&) = delete;
  sem& operator=(sem&&) = delete;
};

///
/// @brief A counting semaphore class borrowing the native sem.
///
class borrowed_sem : public sem_base<borrowed_sem> {
public:
  ///
  /// @brief Contruct a sem using a native k_sem*
  ///
  /// @param s The k_sem to use. @a s must already be
  ///          initialized and will not be freed.
  ///
  explicit constexpr borrowed_sem(struct k_sem* s) noexcept
    : m_sem_ptr(s)
  {
    __ASSERT_NO_MSG(m_sem_ptr != nullptr);
  }

  ///
  /// @brief get the native zephyr sem handle.
  ///
  /// @return A pointer to the zephyr k_sem.
  ///
  constexpr auto native_handle() noexcept
  {
    __ASSERT_NO_MSG(m_sem_ptr != nullptr);

    return m_sem_ptr;
  }
private:
  struct k_sem* m_sem_ptr{ nullptr };
public:
  borrowed_sem(const borrowed_sem&) = delete;
  borrowed_sem(borrowed_sem&&) = delete;
  borrowed_sem& operator=(const borrowed_sem&) = delete;
  borrowed_sem& operator=(borrowed_sem&&) = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_SEM_HPP
