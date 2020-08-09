//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_POLL_SIGNAL_HPP
#define ZPP_INCLUDE_ZPP_POLL_SIGNAL_HPP

#ifdef CONFIG_POLL

#include <kernel.h>
#include <sys/__assert.h>

#include <limits>
#include <optional>

namespace zpp {

///
/// @brief CRTP base class for poll_signals
///
/// @param PollSignal the CRTP derived class
///
template<typename PollSignal>
class poll_signal_base {
protected:
  ///
  /// @brief default contructor only to be used by derived classes
  ///
  poll_signal_base() noexcept
  {
  }
public:
  ///
  /// @brief check if the signal was signaled
  ///
  /// @return when the signal was signaled the signal value
  ///
  std::optional<int> check() noexcept
  {
    unsigned int signaled{0};
    int result{0};

    k_poll_signal_check(native_handle(), &signaled, &result);

    if (signaled != 0) {
      return { result };
    } else {
      return {};
    }
  }

  ///
  /// @brief signal the signal
  ///
  /// @param result the value that check will return
  ///
  /// @return false if the signalling failed
  ///
  bool raise(int result) noexcept
  {
    auto rc = k_poll_signal_raise(native_handle(), result);
    if (rc == 0) {
      return true;
    } else {
      return false;
    }
  }

  ///
  /// @brief reset the state to non-signalled
  ///
  void reset() noexcept
  {
    k_poll_signal_reset(native_handle());
  }

  ///
  /// @brief get the native k_poll_signal handle
  ///
  /// @return pointer to a k_poll_signal
  ///
  auto native_handle() noexcept
  {
    return static_cast<PollSignal*>(this)->native_handle();
  }
public:
  poll_signal_base(const poll_signal_base&) = delete;
  poll_signal_base(poll_signal_base&&) = delete;
  poll_signal_base& operator=(const poll_signal_base&) = delete;
  poll_signal_base& operator=(poll_signal_base&&) = delete;
};

///
/// @brief class owning a k_poll_signal
///
class poll_signal
  : public poll_signal_base<poll_signal>
{
public:
  ///
  /// @brief default constructor initializing the signal
  ///
  poll_signal() noexcept
  {
    k_poll_signal_init(&m_signal);
  }

  ///
  /// @brief get the native k_poll_signal handle
  ///
  /// @return pointer to a k_poll_signal
  ///
  auto native_handle() noexcept
  {
    return &m_signal;
  }
private:
  struct k_poll_signal m_signal;
public:
  poll_signal(const poll_signal&) = delete;
  poll_signal(poll_signal&&) = delete;
  poll_signal& operator=(const poll_signal&) = delete;
  poll_signal& operator=(poll_signal&&) = delete;
};

///
/// @brief class wrapping a k_poll_signal
///
class borrowed_poll_signal
  : public poll_signal_base<borrowed_poll_signal>
{
public:
  ///
  /// @brief create a wrapper around a k_poll_signal
  ///
  /// @param s the signal to wrap
  ///
  borrowed_poll_signal(k_poll_signal* s) noexcept
    : m_signal_ptr(s)
  {
  }

  ///
  /// @brief get the native k_poll_signal handle
  ///
  /// @return pointer to a k_poll_signal
  ///
  auto native_handle() noexcept
  {
    return m_signal_ptr;
  }
private:
  struct k_poll_signal* m_signal_ptr{ nullptr };
public:
  borrowed_poll_signal() = delete;
  borrowed_poll_signal(const borrowed_poll_signal&) = delete;
  borrowed_poll_signal(borrowed_poll_signal&&) = delete;
  borrowed_poll_signal& operator=(const borrowed_poll_signal&) = delete;
  borrowed_poll_signal& operator=(borrowed_poll_signal&&) = delete;
};

} // namespace zpp

#endif // CONFIG_POLL

#endif // ZPP_INCLUDE_ZPP_POLL_SIGNAL_HPP
