/*
* Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr/ztest.h>
#include <zephyr/kernel.h>

#include <zpp/clock.hpp>
#include <zpp/fmt.hpp>
#include <zpp/thread.hpp>

ZTEST_SUITE(zpp_clock_tests, NULL, NULL, NULL, NULL, NULL);

ZTEST(zpp_clock_tests, test_clock_uptime)
{
  using namespace std::chrono;

  auto start = zpp::uptime_clock::now();

  zpp::this_thread::sleep_for(2s);

  auto end = zpp::uptime_clock::now();

  zassert_true(end > start, "end time not later than start time");
}
