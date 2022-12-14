//
// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_THREAD_STACK_HPP
#define ZPP_INCLUDE_ZPP_THREAD_STACK_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/arch_interface.h>
#include <zephyr/sys/__assert.h>

namespace zpp {

///
/// @brief thread_stack holds the stack and thread control block memory
///
/// @param StackSize defines the stacksize in bytes
///
class thread_stack {
public:
  constexpr thread_stack(const thread_stack&) noexcept = default;
  constexpr thread_stack& operator=(const thread_stack&) noexcept = default;

  constexpr thread_stack(k_thread_stack_t* data, size_t size) noexcept
    : m_data(data)
    , m_size(size)
  {
    __ASSERT_NO_MSG(m_data != nullptr);
  }

  constexpr auto size() const noexcept
  {
    return m_size;
  }

  constexpr auto data() const noexcept
  {
    return m_data;
  }
private:
  k_thread_stack_t* m_data;
  size_t m_size;
public:
  thread_stack() = delete;
};

#define ZPP_THREAD_STACK_DEFINE(sym, size)                        \
  K_THREAD_STACK_DEFINE(sym##_native,(size));                     \
  consteval auto sym() noexcept {                                 \
    return zpp::thread_stack(sym##_native, size);                 \
  }

#define ZPP_THREAD_PINNED_STACK_DEFINE(sym, size)                 \
  K_THREAD_PINNED_STACK_DEFINE(sym##_native,(size));              \
  consteval auto sym() noexcept {                                 \
    return zpp::thread_stack(sym##_native, size);                 \
  }

#define ZPP_THREAD_STACK_ARRAY_DEFINE(sym, nmemb, size)           \
  K_THREAD_STACK_ARRAY_DEFINE(sym##_native, nmemb, size);         \
  constexpr auto sym(size_t n) noexcept {                         \
    return zpp::thread_stack(sym##_native[n], size);              \
  }

#define ZPP_THREAD_PINNED_STACK_ARRAY_DEFINE(sym, nmemb, size)    \
  K_THREAD_PINNED_STACK_ARRAY_DEFINE(sym##_native, nmemb, size);  \
  constexpr auto sym(size_t n) noexcept {                         \
    return zpp::thread_stack(sym##_native[n], size);              \
  }


#define ZPP_KERNEL_STACK_DEFINE(sym, size)                        \
  K_KERNEL_STACK_DEFINE(sym##_native,(size));                     \
  consteval auto sym() noexcept {                                 \
    return zpp::thread_stack(sym##_native, size);                 \
  }

#define ZPP_KERNEL_PINNED_STACK_DEFINE(sym, size)                 \
  K_KERNEL_PINNED_STACK_DEFINE(sym##_native,(size));              \
  consteval auto sym() noexcept {                                 \
    return zpp::thread_stack(sym##_native, size);                 \
  }

#define ZPP_KERNEL_STACK_ARRAY_DEFINE(sym, nmemb, size)           \
  K_KERNEL_STACK_ARRAY_DEFINE(sym##_native, nmemb, size);         \
  constexpr auto sym(size_t n) noexcept {                         \
    return zpp::thread_stack(sym##_native[n], size);              \
  }

#define ZPP_KERNEL_PINNED_STACK_ARRAY_DEFINE(sym, nmemb, size)    \
  K_KERNEL_PINNED_STACK_ARRAY_DEFINE(sym##_native, nmemb, size);  \
  constexpr auto sym(size_t n) noexcept {                         \
    return zpp::thread_stack(sym##_native[n], size);              \
  }

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_THREAD_STACK_HPP
