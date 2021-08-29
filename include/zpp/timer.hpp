//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_TIMER_HPP
#define ZPP_INCLUDE_ZPP_TIMER_HPP

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <functional>
#include <type_traits>

#include <zpp/clock.hpp>

namespace zpp {

///
/// @brief base class for the timer class
///
class timer_base {
protected:
  timer_base() noexcept
  {
  }
public:
  ///
  /// @brief Destructor that stops the timer
  ///
  ~timer_base()
  {
    stop();
  }

  ///
  /// @brief Start a timer with @a duration and @a period.
  ///
  /// @param duration The first time out
  /// @param period the time of the repeat period
  ///
  template<class T_Rep1, class T_Period1, class T_Rep2, class T_Period2>
  void start(const std::chrono::duration<T_Rep1, T_Period1>& duration,
       const std::chrono::duration<T_Rep2, T_Period2>& period) noexcept
  {
    using namespace std::chrono;

    k_timer_start(&m_timer, to_timeout(duration), to_timeout(period));
  }

  ///
  /// @brief Start a single shot timer with @a duration
  ///
  /// @param duration The timeout
  ///
  template<class T_Rep, class T_Period>
  void start(const std::chrono::duration<T_Rep, T_Period>& duration) noexcept
  {
    using namespace std::chrono;

    k_timer_start(&m_timer, to_timeout(duration), K_NO_WAIT);
  }

  ///
  /// @brief Stop the timer
  ///
  void stop () noexcept
  {
    k_timer_stop(&m_timer);
  }

  ///
  /// @brief get the timer status
  ///
  /// @return the timer status
  ///
  auto status() noexcept
  {
    return k_timer_status_get(&m_timer);
  }

  ///
  /// @brief sync with the timer
  ///
  auto sync() noexcept
  {
    return k_timer_status_sync(&m_timer);
  }

  ///
  /// @brief Get remaining time
  ///
  /// @return the remaining time
  ///
  std::chrono::nanoseconds remaining_time() noexcept
  {
    auto t = k_timer_remaining_ticks(&m_timer);
    return std::chrono::nanoseconds(k_ticks_to_ns_floor64(t));
  }

  ///
  /// @brief Zephyr native handle.
  ///
  /// @return pointer to the k_timer
  ///
  auto native_handle() noexcept
  {
    return &m_timer;
  }
private:
  struct k_timer m_timer { };
public:
  timer_base(const timer_base&) = delete;
  timer_base(timer_base&&) = delete;
  timer_base& operator=(const timer_base&) = delete;
  timer_base& operator=(timer_base&&) = delete;
};

///
/// @brief timer class with expire and stop callbacks
///
/// @param ExpireCallback Type of the expire callback
/// @param StopCallback Type of the stop callback
///
template<class T_ExpireCallback, class T_StopCallback>
class timer : public timer_base
{
public:
  timer() = delete;

  ///
  /// @brief construct timer with expire and stop callback
  ///
  /// @param ecb the expire callback
  /// @param scb the stop callbacl
  ///
  explicit timer(T_ExpireCallback ecb, T_StopCallback scb) noexcept
    : timer_base()
    , m_expire_callback(ecb)
    , m_stop_callback(scb)
  {
    k_timer_expiry_t ecb_func = [](struct k_timer* t) noexcept {
      auto self = get_user_data(t);
      if (self != nullptr) {
        std::invoke(self->m_expire_callback, self);
      }
    };

    k_timer_stop_t scb_func = [](struct k_timer* t) noexcept {
      auto self = get_user_data(t);
      if (self != nullptr) {
        std::invoke(self->m_stop_callback, self);
      }
    };

    k_timer_init( native_handle(), ecb_func, scb_func);
    k_timer_user_data_set( native_handle(), this);
  }
private:
  static timer* get_user_data(struct k_timer* t) noexcept
  {
    return static_cast<timer*>(k_timer_user_data_get(t));
  }
private:
  T_ExpireCallback  m_expire_callback;
  T_StopCallback    m_stop_callback;
};


///
/// @brief timer class with only an expire callback
///
/// @param ExpireCallback Type of the expire callback
///
template<class T_ExpireCallback>
class basic_timer : public timer_base
{
public:
  basic_timer() = delete;

  ///
  /// @brief construct timer with an expire callback
  ///
  /// @param ecb the expire callback
  ///
  explicit basic_timer(T_ExpireCallback ecb) noexcept
    : timer_base()
    , m_expire_callback(ecb)
  {
    k_timer_expiry_t ecb_func = [](struct k_timer* t) noexcept {
      auto self = get_user_data(t);
      if (self != nullptr) {
        std::invoke(self->m_expire_callback, self);
      }
    };

    k_timer_init( native_handle(), ecb_func, nullptr);
    k_timer_user_data_set( native_handle(), this);
  }
private:
  static basic_timer* get_user_data(struct k_timer* t) noexcept
  {
    return static_cast<basic_timer*>(k_timer_user_data_get(t));
  }
private:
  T_ExpireCallback  m_expire_callback;
};


///
/// @brief timer class with no callbacks used for syncing only
///
class sync_timer : public timer_base
{
public:
  ///
  /// @brief constuctor for the sync timer
  ///
  sync_timer() noexcept
    : timer_base()
  {
    k_timer_init(native_handle(), nullptr, nullptr);
  }
};

///
/// @brief create sync_timer object
///
/// @return sync_timer object
///
inline auto make_timer() noexcept
{
  return sync_timer();
}

///
/// @brief create basic_timer object
///
/// @param ecb the expire callback
///
/// @return basic_timer object
///
template<class T_ExpireCallback>
inline auto make_timer(T_ExpireCallback&& ecb) noexcept
{
  return basic_timer(std::forward<T_ExpireCallback>(ecb));
}

///
/// @brief create timer object
///
/// @param ecb the expire callback
/// @param scb the stop callback
///
/// @return timer object
///
template<class T_ExpireCallback, class T_StopCallback>
inline auto make_timer(T_ExpireCallback&& ecb, T_StopCallback&& scb) noexcept
{
  return timer(std::forward<T_ExpireCallback>(ecb), std::forward<T_StopCallback>(scb));
}

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_TIMER_HPP
