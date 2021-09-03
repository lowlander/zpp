//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP_INCLUDE_ZPP_HPP
#define ZPP_INCLUDE_ZPP_HPP

#ifndef __cplusplus
#error "ZPP is a C++ only library"
#else
#if __cplusplus < 201709L
#error "ZPP needs C++20 or newer"
#endif
#endif

#include <zpp/error_code.hpp>
#include <zpp/result.hpp>
#include <zpp/atomic_bitset.hpp>
#include <zpp/atomic_var.hpp>
#include <zpp/clock.hpp>
#include <zpp/condition_variable.hpp>
#include <zpp/fmt.hpp>
#include <zpp/fifo.hpp>
#include <zpp/heap.hpp>
#include <zpp/mem_slab.hpp>
#include <zpp/futex.hpp>
#include <zpp/mutex.hpp>
#include <zpp/sys_mutex.hpp>
#include <zpp/poll.hpp>
#include <zpp/sched.hpp>
#include <zpp/sem.hpp>
#include <zpp/thread.hpp>
#include <zpp/timer.hpp>
#include <zpp/lock_guard.hpp>
#include <zpp/utils.hpp>
#include <zpp/unique_lock.hpp>

#endif // ZPP_INCLUDE_ZPP_HPP
