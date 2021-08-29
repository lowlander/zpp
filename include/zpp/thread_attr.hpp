//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_THREAD_ATTR_HPP
#define ZPP_INCLUDE_ZPP_THREAD_ATTR_HPP

#include <zpp/thread_prio.hpp>

#include <kernel.h>
#include <sys/__assert.h>

#include <chrono>
#include <utility>

namespace zpp {

enum class thread_suspend { yes, no };
enum class thread_essential { yes, no };
enum class thread_user { yes, no };
enum class thread_inherit_perms { yes, no };
enum class thread_fp_regs { yes, no };
enum class thread_sse_regs { yes, no };

template<class T_Rep, class T_Period>
struct thread_start_delay {
  std::chrono::duration<T_Rep, T_Period> t;
};

///
/// @brief Thread creation attributes.
///
class thread_attr {
public:
  thread_attr(const thread_attr&) noexcept = default;
  thread_attr(thread_attr&&) noexcept = default;
  thread_attr& operator=(const thread_attr&) noexcept = default;
  thread_attr& operator=(thread_attr&&) noexcept = default;

  ///
  /// @brief Constructor taking a variable number of attributes
  ///
  /// Contructor that initializes the thread creation attributes
  /// based on the @a args arguments being passed. The order of
  /// the arguments does not matter. Arguments must be of a Type
  /// that has a coresponding set(Type) function.
  ///
  /// @param args The attributes to set.
  ///
  template <class ...T_Args>
  constexpr explicit thread_attr(T_Args&&... args) noexcept
  {
    set(std::forward<T_Args>(args)...);
  }

  template <class T_FirstArg, class ...T_Args>
  constexpr void set(T_FirstArg&& first, T_Args&&... rest) noexcept
  {
    set(std::forward<T_FirstArg>(first));
    set(std::forward<T_Args>(rest)...);
  }

  constexpr void set() const noexcept
  {
  }

  ///
  /// @brief Set the thread priority
  ///
  /// @param prio The thread priority
  ///
  constexpr void set(thread_prio prio) noexcept
  {
    m_prio = prio;
  }

  ///
  /// @brief Set the thread start delay
  ///
  /// @param delay The thread start delay
  ///
  template<class T_Rep, class T_Period>
  constexpr void
  set(const thread_start_delay<T_Rep, T_Period>& delay) noexcept
  {
    using namespace std::chrono;

    //auto ms = duration_cast<milliseconds>(delay.t);

    if (delay.t <= decltype(delay.t)::zero()) {
      m_delay = K_NO_WAIT;
    } else {
      m_delay = to_tick(delay.t);
    }
  }

  ///
  /// @brief Set the thread start suspended flag
  ///
  /// @param v The thread start suspended flag
  ///
  constexpr void set(thread_suspend v) noexcept
  {
    if (v == thread_suspend::yes) {
      m_delay = K_FOREVER;
    } else {
      if (K_TIMEOUT_EQ(m_delay, K_FOREVER)) {
        m_delay = K_NO_WAIT;
      }
    }
  }

  ///
  /// @brief Set the thread essential flag
  ///
  /// @param v The thread essential flag
  ///
  constexpr void set(thread_essential v) noexcept
  {
    if (v == thread_essential::yes) {
      m_options |= K_ESSENTIAL;
    } else {
      m_options &= ~K_ESSENTIAL;
    }
  }

  ///
  /// @brief Set the thread user flag
  ///
  /// @param v The thread user flag
  ///
  constexpr void set(thread_user v) noexcept
  {
    if (v == thread_user::yes) {
      m_options |= K_USER;
    } else {
      m_options &= ~K_USER;
    }
  }

  ///
  /// @brief Set the thread inherit permisions flag
  ///
  /// @param v The thread inherit permissions flag
  ///
  constexpr void set(thread_inherit_perms v) noexcept
  {
    if (v == thread_inherit_perms::yes) {
      m_options |= K_INHERIT_PERMS;
    } else {
      m_options &= ~K_INHERIT_PERMS;
    }
  }

  ///
  /// @brief Set the thread FP regs flag
  ///
  /// @param v The thread FP regs flag
  ///
  constexpr void set(thread_fp_regs v) noexcept
  {
#ifdef K_FP_REGS
    if (v == thread_fp_regs::yes) {
      m_options |= K_FP_REGS;
    } else {
      m_options &= ~K_FP_REGS;
    }
#endif
  }

  ///
  /// @brief Set the thread SSE regs flag
  ///
  /// @param v The thread SSE regs flag
  ///
  constexpr void set(thread_sse_regs v) noexcept
  {
#ifdef K_SSE_REGS
    if (v == thread_sse_regs::yes) {
      m_options |= K_SSE_REGS;
    } else {
      m_options &= ~K_SSE_REGS;
    }
#endif
  }

  ///
  /// @brief get the Zephyr native delay value
  ///
  /// @return The native delay value
  ///
  constexpr auto native_delay() const noexcept
  {
    return m_delay;
  }

  ///
  /// @brief get the Zephyr native priority value
  ///
  /// @return The native priority value
  ///
  constexpr auto native_prio() const noexcept
  {
    return m_prio.native_value();
  }

  ///
  /// @brief get the Zephyr native options value
  ///
  /// @return The native options value
  ///
  constexpr auto native_options() const noexcept
  {
    return m_options;
  }
private:
  thread_prio m_prio{ };
  uint32_t    m_options{ 0 };
  k_timeout_t m_delay{ K_NO_WAIT };
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_THREAD_ATTR_HPP
