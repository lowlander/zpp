/*
 * Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/sem.hpp>
#include <zpp/thread.hpp>

namespace {

K_SEM_DEFINE(bm_sem, 0, 10);

zpp::borrowed_sem simple_borrowed_sem(&bm_sem);

zpp::sem simple_sem(0, 10);

void test_sem_take()
{
	simple_sem.reset();

	for (int i = 0; i < 5; i++) {
		simple_sem++;

		auto signal_count = simple_sem.count();
		zassert_true(signal_count == (i + 1),
			     "signal count missmatch Expected %d, got %d\n",
			     (i + 1), signal_count);
	}
}

void test_sem_try_take()
{
	for (int i = 4; i >= 0; i--) {
		auto ret_value = simple_sem.try_take();
		zassert_true(ret_value == true,
			     "unable to do k_sem_take which returned %d\n",
			     ret_value);

		auto signal_count = simple_sem.count();
		zassert_true(signal_count == i,
			     "signal count missmatch Expected %d, got %d\n",
			     i, signal_count);
	}
}

void test_sem_try_take_fails()
{
	simple_sem.reset();

	for (int i = 4; i >= 0; i--) {
		auto ret_value = simple_sem.try_take();
		zassert_true(ret_value == false,
			     "k_sem_take returned when not possible");

		auto signal_count = simple_sem.count();
		zassert_true(signal_count == 0U,
			     "signal count missmatch Expected 0, got %d\n",
			     signal_count);
	}
}

void test_sem_try_take_for_fails()
{
	using namespace std::chrono;

	simple_sem.reset();

	for (int i = 4; i >= 0; i--) {
		auto ret_value = simple_sem.try_take_for(100ms);
		zassert_true(ret_value == false,
			     "k_sem_take succeeded when its not possible");
	}
}

void test_sem_take_borrowed()
{
	simple_borrowed_sem.reset();

	for (int i = 0; i < 5; i++) {
		simple_borrowed_sem++;

		auto signal_count = simple_borrowed_sem.count();
		zassert_true(signal_count == (i + 1),
			     "signal count missmatch Expected %d, got %d\n",
			     (i + 1), signal_count);
	}
}

void test_sem_try_take_borrowed()
{
	for (int i = 4; i >= 0; i--) {
		auto ret_value = simple_borrowed_sem.try_take();
		zassert_true(ret_value == true,
			     "unable to do k_sem_take which returned %d\n",
			     ret_value);

		auto signal_count = simple_borrowed_sem.count();
		zassert_true(signal_count == i,
			     "signal count missmatch Expected %d, got %d\n",
			     i, signal_count);
	}
}

void test_sem_try_take_fails_borrowed()
{
	simple_borrowed_sem.reset();

	for (int i = 4; i >= 0; i--) {
		auto ret_value = simple_borrowed_sem.try_take();
		zassert_true(ret_value == false,
			     "k_sem_take returned when not possible");

		auto signal_count = simple_borrowed_sem.count();
		zassert_true(signal_count == 0U,
			     "signal count missmatch Expected 0, got %d\n",
			     signal_count);
	}
}

void test_sem_try_take_for_fails_borrowed()
{
	using namespace std::chrono;

	simple_borrowed_sem.reset();

	for (int i = 4; i >= 0; i--) {
		auto ret_value = simple_borrowed_sem.try_take_for(100ms);
		zassert_true(ret_value == false,
			     "k_sem_take succeeded when its not possible");
	}
}

} // namespace

void test_main(void)
{
	ztest_test_suite(test_zpp_sem,
			 ztest_unit_test(test_sem_take),
			 ztest_unit_test(test_sem_try_take),
			 ztest_unit_test(test_sem_try_take_fails),
			 ztest_unit_test(test_sem_try_take_for_fails),
			 ztest_unit_test(test_sem_take_borrowed),
			 ztest_unit_test(test_sem_try_take_borrowed),
			 ztest_unit_test(test_sem_try_take_fails_borrowed),
			 ztest_unit_test(test_sem_try_take_for_fails_borrowed)
			);
	ztest_run_test_suite(test_zpp_sem);
}
