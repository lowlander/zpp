/*
 * Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/mutex.hpp>
#include <zpp/thread.hpp>

namespace {

zpp::mutex m;

K_MUTEX_DEFINE(bm_mutex);

zpp::borrowed_mutex bm(&bm_mutex);

void test_mutex()
{
	m.lock();

	m.unlock();
}

void test_borrowed_mutex()
{
	bm.lock();

	bm.unlock();
}

void test_lock_guard()
{
	zpp::lock_guard g(m);
	zpp::lock_guard bg(bm);
}

} // namespace

void test_main(void)
{
	ztest_test_suite(test_zpp_mutex,
			 ztest_unit_test(test_mutex),
			 ztest_unit_test(test_borrowed_mutex),
			 ztest_unit_test(test_lock_guard)
			);
	ztest_run_test_suite(test_zpp_mutex);
}
