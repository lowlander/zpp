//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_POLL_EVENT_SET_HPP
#define ZPP_INCLUDE_ZPP_POLL_EVENT_SET_HPP

#ifdef CONFIG_POLL

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>

#include <chrono>
#include <limits>
#include <optional>
#include <array>
#include <utility>

#include <zpp/clock.hpp>

#include <zpp/sem.hpp>
#include <zpp/fifo.hpp>

#include <zpp/poll_event.hpp>
#include <zpp/poll_signal.hpp>

namespace zpp {

///
/// @brief A set of poll events
///
/// @param Size the size of the set
///
template<int T_Size>
class poll_event_set
{
public:
  auto begin() noexcept { return m_events.begin(); }
  auto end() noexcept { return m_events.end(); }
public:
  ///
  /// @brief default constructor
  ///
  poll_event_set() noexcept
  {
  }

  ///
  /// @brief constructor that takes arguments for initialization
  ///
  /// @param t the arguments to use for initialization
  ///
  template <class... T_Args>
  poll_event_set(T_Args&&... t) noexcept
  {
    assign(0, std::forward<T_Args>(t)...);
  }

  ///
  /// @brief access an element of the set
  ///
  /// @param idx the index of the event to get
  ///
  /// @return a poll_event that has a reference to the indexed event
  ///
  auto operator[](size_t idx) noexcept
  {
    __ASSERT_NO_MSG(idx < T_Size);
    return poll_event(&m_events[idx]);
  }

  ///
  /// @brief poll events waiting for ever
  ///
  /// @return false on error
  ///
  auto poll() noexcept
  {
    return poll(K_FOREVER);
  }

  ///
  /// @brief try poll events without waiting
  ///
  /// @return false on error
  ///
  auto try_poll() noexcept
  {
    return poll(K_NO_WAIT);
  }

  ///
  /// @brief try poll events waiting for e certain time
  ///
  /// @param timeout the time to wait
  ///
  /// @return false on error
  ///
  template<class T_Rep, class T_Period>
  auto try_poll_for(const std::chrono::duration<T_Rep, T_Period>&
            timeout) noexcept
  {
    using namespace std::chrono;

    return poll(to_timeout(timeout));
  }
private:
  ///
  /// @brief try poll events waiting for e certain time
  ///
  /// @param timeout the time to wait
  ///
  /// @return false on error
  ///
  void assign(int index) noexcept
  {
    __ASSERT_NO_MSG(index == T_Size);
  }

  ///
  /// @brief try poll events waiting for e certain time
  ///
  /// @param timeout the time to wait
  ///
  /// @return false on error
  ///
  template<class T_FirstArg, class... T_Args>
  void assign(int index, T_FirstArg&& f, T_Args&&... t) noexcept
  {
    poll_event(&m_events[index]).assign(std::forward<T_FirstArg>(f));
    assign(index+1, std::forward<T_Args>(t)...);
  }

  ///
  /// @brief try poll events waiting for e certain time
  ///
  /// @param timeout the time to wait
  ///
  /// @return false on error
  ///
  auto poll(k_timeout_t timeout) noexcept
  {
    for (auto& e: m_events) {
      e.state = K_POLL_STATE_NOT_READY;
      if (e.tag == (int)poll_event::type_tag::type_signal) {
        __ASSERT_NO_MSG(e.signal != nullptr);
        e.signal->signaled = 0;
      }
    }

    auto rc = k_poll(m_events.data(), m_events.size(), timeout);

    if (rc == 0) {
      return true;
    } else {
      return false;
    }
  }
private:
  std::array<struct k_poll_event, T_Size> m_events;
};

///
/// @brief try poll events waiting for e certain time
///
/// @param timeout the time to wait
///
/// @return false on error
///
template <class... T_Args>
poll_event_set(T_Args&&... t) noexcept -> poll_event_set<sizeof...(T_Args)>;

} // namespace zpp

#endif // CONFIG_POLL

#endif // ZPP_INCLUDE_ZPP_POLL_EVENT_SET_HPP
