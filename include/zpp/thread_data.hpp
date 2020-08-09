//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_THREAD_DATA_HPP
#define ZPP_INCLUDE_ZPP_THREAD_DATA_HPP

#include <kernel.h>
#include <sys/__assert.h>

namespace zpp {

///
/// @brief thread_data holds the stack and thread control block memory
///
/// @param StackSize defines the stacksize in bytes
///
template <uint32_t StackSize>
class thread_data {
public:
	//
	// @brief Default constructor
	//
	thread_data() noexcept
	{
	}
private:
	friend class thread;

	auto stack_data() noexcept
	{
		return m_thread_stack;
	}

	auto stack_size() const noexcept
	{
		return K_THREAD_STACK_SIZEOF(m_thread_stack);
	}

	auto native_thread_ptr() noexcept {
		return &m_thread_data;
	}
private:
	struct k_thread m_thread_data;
	K_THREAD_STACK_MEMBER(m_thread_stack, StackSize);
public:
	thread_data(const thread_data&) = delete;
	thread_data(thread_data&&) = delete;
	thread_data& operator=(const thread_data&) = delete;
	thread_data& operator=(thread_data&&) = delete;
};

} // namespace zpp

#endif // ZPP_INCLUDE_ZPP_THREAD_DATA_HPP
