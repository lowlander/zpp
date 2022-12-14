//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_THREAD_DATA_HPP
#define ZPP_INCLUDE_ZPP_THREAD_DATA_HPP

#include <zephyr/kernel.h>
#include <zephyr/sys/arch_interface.h>
#include <zephyr/sys/__assert.h>

namespace zpp {

///
/// @brief thread_data holds the stack and thread control block memory
///
class thread_data {
public:
  //
  // @brief Default constructor
  //
  constexpr thread_data() noexcept = default;

  constexpr auto native_handle() noexcept -> struct k_thread*
  {
    return &m_thread_data;
  }
private:
  struct k_thread m_thread_data;
public:
  thread_data(const thread_data&) = delete;
  thread_data(thread_data&&) = delete;
  thread_data& operator=(const thread_data&) = delete;
  thread_data& operator=(thread_data&&) = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_THREAD_DATA_HPP
