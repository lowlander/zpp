/*
 * Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/clock.hpp>
#include <zpp/fmt.hpp>
#include <zpp/thread.hpp>

namespace {

void test_clock_uptime()
{
	using namespace std::chrono;

	auto start = zpp::uptime_clock::now();

	zpp::this_thread::sleep_for(2s);

	auto end = zpp::uptime_clock::now();

	zassert_true(end > start, "end time not later than start time");
}

} // namespace

void test_main()
{
	ztest_test_suite(zpp_clock_tests,
			 ztest_unit_test(test_clock_uptime)
		);

	ztest_run_test_suite(zpp_clock_tests);
}
