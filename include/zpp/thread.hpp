//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_THREAD_HPP
#define ZPP_INCLUDE_ZPP_THREAD_HPP

#include <zpp/thread_prio.hpp>
#include <zpp/thread_id.hpp>
#include <zpp/thread_attr.hpp>
#include <zpp/thread_data.hpp>
#include <zpp/thread_stack.hpp>
#include <zpp/clock.hpp>
#include <zpp/result.hpp>
#include <zpp/error_code.hpp>
#include <zpp/heap.hpp>

#include <zephyr/kernel.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/sys/util.h>

#include <functional>
#include <chrono>
#include <tuple>
#include <utility>
#include <memory>

namespace zpp {

///
/// @brief provide functions that access the current thread of execution.
///
namespace this_thread {

///
/// @brief Get the thread ID of the current thread
///
/// @return The thread ID of the current thread.
///
inline auto get_id() noexcept
{
  return ::zpp::thread_id(k_current_get());
}

///
/// @brief Yield the current thread.
///
inline void yield() noexcept
{
  k_yield();
}

///
/// @brief Busy wait for a specified time duration
///
/// @param wait_duration The time to busy wait
///
template<class T_Rep, class T_Period>
inline void
busy_wait_for(const std::chrono::duration<T_Rep, T_Period>& wait_duration)
{
  using namespace std::chrono;

  microseconds us = duration_cast<microseconds>(wait_duration);
  k_busy_wait(us.count());
}

///
/// @brief Suspend the current thread for a specified time duration
///
/// @param wait_duration The time to sleep
///
template<class T_Rep, class T_Period >
inline auto
sleep_for(const std::chrono::duration<T_Rep, T_Period>& sleep_duration)
{
  auto res = k_sleep(to_timeout(sleep_duration));

  return std::chrono::milliseconds(res);
}

///
/// @brief Suspend the current thread until a specified time point
///
/// @param wait_duration The time point util the current thread will sleep
///
template<class T_Clock, class T_Duration>
inline void
sleep_until( const std::chrono::time_point<T_Clock, T_Duration>& sleep_time)
{
  using namespace std::chrono;

  T_Duration dt;
  while ( (dt = sleep_time - T_Clock::now()) > T_Duration::zero()) {
    k_sleep(to_timeout(dt));
  }
}

///
/// @brief Abort the current thread
///
inline void abort() noexcept
{
  k_thread_abort(k_current_get());
}

///
/// @brief Suspend the current thread
///
inline void suspend() noexcept
{
  k_thread_suspend(k_current_get());
}

///
/// @brief Get the current thread's priority
///
/// @return The priority of the current thread
///
inline thread_prio get_priority() noexcept
{
  return thread_prio( k_thread_priority_get(k_current_get()) );
}

///
/// @brief Set the current thread's priority
///
/// @param prio The new priority of the current thread
///
inline void set_priority(thread_prio prio) noexcept
{
  k_thread_priority_set(k_current_get(), prio.native_value());
}

} // namespace this_thread

template <class T> typename std::decay<T>::type decay_copy(T&& v) noexcept
{
  return std::forward<T>(v);
}

///
/// @brief The class thread repecents a single Zephyr thread.
///
class thread {
private:
  template<typename T_CallInfo>
  static void callback_helper(void* a1, void* a2, void* a3) noexcept
  {
    (void)a2;
    (void)a3;

    auto cip = reinterpret_cast<T_CallInfo*>(a1);
    __ASSERT_NO_MSG(cip != nullptr);

    auto f = std::move(cip->m_f);
    auto args = std::move(cip->m_args);

    auto heap = cip->m_heap;

    std::destroy_at(cip);
    heap->deallocate(cip);

    std::apply(f, std::move(args));
  }

  template<typename T_Callback, typename T_CallbackArg>
  static void callback_helper(void* a1, void* a2, void* a3) noexcept
  {
    (void)a3;

    auto f = reinterpret_cast<T_Callback>(a1);
    __ASSERT_NO_MSG(f != nullptr);

    auto arg = reinterpret_cast<T_CallbackArg*>(&a2);
    __ASSERT_NO_MSG(arg != nullptr);

    std::invoke(*f, *arg);
  }

  template<typename T_Callback>
  static void callback_helper_void(void* a1, void* a2, void* a3) noexcept
  {
    (void)a2;
    (void)a3;

    auto f = reinterpret_cast<T_Callback>(a1);
    __ASSERT_NO_MSG(f != nullptr);

    std::invoke(*f);
  }

public:
  ///
  /// @brief Creates a object which doesn't represent a Zephyr thread.
  ///
  constexpr thread() noexcept
  {
  }

  ///
  /// @brief Creates a object which represents Zephyr thread with tid.
  ///
  /// @param tid The ID to manage
  ///
  constexpr explicit thread(thread_id tid) noexcept
    : m_tid(tid)
  {
  }

  ///
  /// @brief Creates a object which represents a new Zephyr thread.
  ///
  /// @param tcb The TCB to use
  /// @param attr The creation attributes to use
  /// @param f The thread entry point
  /// @param args The arguments to pass to @a f
  ///
  template<typename T_Heap, typename T_Callback, typename... T_CallbackArgs,
            std::enable_if_t<std::is_nothrow_invocable_v<T_Callback, T_CallbackArgs...>, bool> = true
          >
  constexpr thread(
      thread_data& td,
      thread_stack&& tstack,
      const thread_attr& attr,
      T_Heap* heap,
      T_Callback&& f,
      T_CallbackArgs&&... args) noexcept
  {
    typedef typename std::decay<T_Heap>::type CallInfoHeap;
    typedef typename std::decay<T_Callback>::type CallInfoF;
    typedef std::tuple<typename std::decay<T_CallbackArgs>::type...> CallInfoArgs;

    static_assert(std::is_invocable_v<T_Callback, T_CallbackArgs...>);
    static_assert(std::is_nothrow_invocable_v<T_Callback, T_CallbackArgs...>);

    struct call_info {
      CallInfoHeap* m_heap;
      CallInfoF     m_f;
      CallInfoArgs  m_args;
    };

    void* vp = heap->try_allocate(sizeof(call_info), alignof(call_info));

    if (vp != nullptr) {

      auto cip = std::construct_at(reinterpret_cast<call_info*>(vp),
                    heap,
                    decay_copy(std::forward<T_Callback>(f)),
                    decay_copy(std::forward_as_tuple(args...)));

      __ASSERT_NO_MSG(cip != nullptr);

      auto tid = k_thread_create(
            td.native_handle(),
            tstack.data(),
            tstack.size(),
            &callback_helper<call_info>,
            reinterpret_cast<void*>(cip),
            nullptr,
            nullptr,
            attr.native_prio(),
            attr.native_options(),
            attr.native_delay());

      m_tid = thread_id(tid);
    }
  }


  ///
  /// @brief Creates a object which represents a new Zephyr thread.
  ///
  /// @param tcb The TCB to use
  /// @param attr The creation attributes to use
  /// @param f The thread entry point
  /// @param args The arguments to pass to @a f
  ///
  template<typename T_Callback, typename T_CallbackArg,
            std::enable_if_t<std::is_nothrow_invocable_v<T_Callback, T_CallbackArg>, bool> = true
          >
  constexpr thread(
      thread_data& td,
      thread_stack&& tstack,
      const thread_attr& attr,
      T_Callback f,
      T_CallbackArg arg) noexcept
  {
    using func_t = void (*)(T_CallbackArg) noexcept;

    static_assert(sizeof(T_CallbackArg) <= sizeof(void*));
    static_assert(std::is_invocable_v<T_Callback, T_CallbackArg>);
    static_assert(std::is_nothrow_invocable_v<T_Callback, T_CallbackArg>);
    static_assert(std::is_invocable_v<func_t, T_CallbackArg>);
    static_assert(std::is_nothrow_invocable_v<func_t, T_CallbackArg>);
    static_assert(sizeof(func_t) <= sizeof(void*));
    static_assert(alignof(T_CallbackArg) <= alignof(void*));
    static_assert(std::is_trivial_v<T_CallbackArg>);

    func_t fp = f;

    void* arg_vp{};
    std::construct_at(reinterpret_cast<T_CallbackArg*>(&arg_vp), arg);

    auto tid = k_thread_create(
          td.native_handle(),
          tstack.data(),
          tstack.size(),
          &callback_helper<func_t, T_CallbackArg>,
          reinterpret_cast<void*>(fp),
          arg_vp,
          nullptr,
          attr.native_prio(),
          attr.native_options(),
          attr.native_delay());

    m_tid = thread_id(tid);
  }


  ///
  /// @brief Creates a object which represents a new Zephyr thread.
  ///
  /// @param tcb The TCB to use
  /// @param attr The creation attributes to use
  /// @param f The thread entry point
  /// @param args The arguments to pass to @a f
  ///
  template<typename T_Callback,
            std::enable_if_t<std::is_nothrow_invocable_v<T_Callback>, bool> = true
          >
  constexpr thread(
      thread_data& td,
      thread_stack&& tstack,
      const thread_attr& attr,
      T_Callback f) noexcept
  {
    using func_t = void (*)() noexcept;

    static_assert(std::is_invocable_v<T_Callback>);
    static_assert(std::is_nothrow_invocable_v<T_Callback>);
    static_assert(std::is_invocable_v<func_t>);
    static_assert(std::is_nothrow_invocable_v<func_t>);
    static_assert(sizeof(func_t) <= sizeof(void*));

    func_t fp = f;

    auto tid = k_thread_create(
          td.native_handle(),
          tstack.data(),
          tstack.size(),
          &callback_helper_void<func_t>,
          reinterpret_cast<void*>(fp),
          nullptr,
          nullptr,
          attr.native_prio(),
          attr.native_options(),
          attr.native_delay());

    m_tid = thread_id(tid);
  }


  ///
  /// @brief Move constructor
  ///
  /// @param other the thread to move to this thread object, after the
  ///        move @a other will not manage the thread anymore
  ///
  constexpr thread(thread&& other) noexcept
    : m_tid(other.m_tid)
  {
    other.m_tid = thread_id::any();
  }

  ///
  /// @brief Move assignment operator
  ///
  /// @param other the thread to move to this thread object, after the
  ///        move @a other will not manage the thread anymore
  ///
  constexpr thread& operator=(thread&& other) noexcept
  {
    m_tid = other.m_tid;
    other.m_tid = thread_id::any();

    return *this;
  }

  ///
  /// @brief Destructor, will abort the thread.
  ///
  ~thread() noexcept
  {
    if (m_tid) {
      k_thread_abort(m_tid.native_handle());
    }
  }

  ///
  /// @brief check if this object manages a thread
  ///
  /// @return true if this object manages a thread
  ///
  constexpr explicit operator bool() const noexcept {
    return !!m_tid;
  }

  ///
  /// @brief Detach this object from the thread.
  ///
  constexpr void detach() noexcept
  {
    m_tid = thread_id::any();
  }

  ///
  /// @brief wakeup the thread this object mamages.
  ///
  [[nodiscard]] auto wakeup() noexcept
  {
    result<void, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      k_wakeup(m_tid.native_handle());
      res.assign_value();
    }

    return res;
  }

  ///
  /// @brief start the thread this object mamages.
  ///
  [[nodiscard]] auto start() noexcept
  {
    result<void, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      k_thread_start(m_tid.native_handle());
      res.assign_value();
    }

    return res;
  }

  ///
  /// @brief abort the thread this object mamages.
  ///
  [[nodiscard]] auto abort() noexcept
  {
    result<void, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      k_thread_abort(m_tid.native_handle());
      m_tid = thread_id::any();
      res.assign_value();
    }

    return res;
  }

  ///
  /// @brief resume the thread this object mamages.
  ///
  [[nodiscard]] auto resume() noexcept
  {
    result<void, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      k_thread_resume(m_tid.native_handle());
      res.assign_value();
    }

    return res;
  }

  ///
  /// @brief join the thread this object mamages.
  ///
  [[nodiscard]] auto join() noexcept
  {
    result<void, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      auto rc = k_thread_join(m_tid.native_handle(), K_FOREVER);
      if (rc == 0) {
        res.assign_value();
      } else {
        res.assign_error(to_error_code(-rc));
      }
    }

    return res;
  }

  ///
  /// @brief suspend the thread this object mamages.
  ///
  [[nodiscard]] auto suspend() noexcept
  {
    result<void, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      k_thread_suspend(m_tid.native_handle());
      res.assign_value();
    }

    return res;
  }

  ///
  /// @brief Get priority of the thread this object mamages.
  ///
  /// @return Thread priority
  ///
  [[nodiscard]] auto priority() noexcept
  {
    result<thread_prio, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      res = thread_prio( k_thread_priority_get(m_tid.native_handle()) );
    }

    return res;
  }

  ///
  /// @brief Set priority of the thread this object mamages.
  ///
  /// @param prio The new thread priority
  ///
  [[nodiscard]] auto set_priority(thread_prio prio) const noexcept
  {
    result<void, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      k_thread_priority_set(m_tid.native_handle(), prio.native_value());
      res.assign_value();
    }

    return res;
  }

  ///
  /// @brief Set name of the thread this object mamages.
  ///
  /// @param name The new thread name
  ///
  [[nodiscard]] auto set_name(const char* name) noexcept
  {
    result<void, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      auto rc = k_thread_name_set(m_tid.native_handle(), name);
      if (rc == 0) {
        res.assign_value();
      } else {
        res.assign_error(to_error_code(-rc));
      }
    }

    return res;
  }

  ///
  /// @brief Get name of the thread this object mamages.
  ///
  /// @return The thread name or nullptr
  ///
  [[nodiscard]] auto name() const noexcept
  {
    result<const char*, error_code> res(error_result(error_code::k_inval));

    if (m_tid) {
      auto rc = k_thread_name_get(m_tid.native_handle());
      if (rc == nullptr) {
        res.assign_error(error_code::k_notsup); // TODO
      } else {
        res.assign_value(rc);
      }
    }

    return res;
  }
private:
  thread_id	m_tid;
public:
  thread(const thread&) = delete;
  thread& operator=(const thread&) = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_THREAD_HPP
