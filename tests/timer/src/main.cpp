/*
 * Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/timer.hpp>
#include <zpp/thread.hpp>
#include <zpp/fmt.hpp>

namespace {

void timer_callback(zpp::timer_base* t) noexcept
{
	zpp::print("Hello from timer tid={}\n", zpp::this_thread::get_id());
}

void test_timer_creation_function()
{
	using namespace zpp;
	using namespace std::chrono;

	auto t = timer(timer_callback);
	t.start(100ms, 1s);

	this_thread::sleep_for(5s);
}

void test_timer_creation_lambda()
{
	using namespace zpp;
	using namespace std::chrono;

	auto t = timer(
		[] (auto t) {
			print("Hello from timer tid={}\n",
					this_thread::get_id());
		} );

	t.start(100ms, 1s);

	this_thread::sleep_for(5s);
}

} // namespace

void test_main()
{
	ztest_test_suite(zpp_timer_tests,
			 ztest_unit_test(test_timer_creation_function),
			 ztest_unit_test(test_timer_creation_lambda)
		);

	ztest_run_test_suite(zpp_timer_tests);
}
