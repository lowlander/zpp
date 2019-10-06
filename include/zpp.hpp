//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef ZPP__INCLUDE__ZPP_HPP
#define ZPP__INCLUDE__ZPP_HPP

#ifndef __cplusplus
#error "ZPP is a C++ only library"
#else
#if __cplusplus < 201703L
#error "ZPP needs C++17 or newer"
#endif
#endif

#include <zpp/atomic_bitset.hpp>
#include <zpp/atomic_var.hpp>
#include <zpp/clock.hpp>
#include <zpp/fmt.hpp>
#include <zpp/fifo.hpp>
#include <zpp/mutex.hpp>
#include <zpp/poll.hpp>
#include <zpp/sched.hpp>
#include <zpp/sem.hpp>
#include <zpp/thread.hpp>
#include <zpp/timer.hpp>

#endif // ZPP__INCLUDE__ZPP_HPP
