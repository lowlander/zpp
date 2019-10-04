/*
 * Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

#include <zpp/thread.hpp>
#include <zpp/sem.hpp>
#include <zpp/fmt.hpp>

#include <chrono>

namespace {

zpp::thread_data<1024> tcb;

} // namespace

static void test_thread_creation(void)
{
	using namespace zpp;
	using namespace std::chrono;

	const thread_attr attr(
				thread_prio::preempt(0),
				thread_inherit_perms::no,
				thread_essential::no,
				thread_suspend::no
			);

	sem done;

	auto t = thread(
		tcb, attr,
		[&done]() {
			print("Hello from thread tid={}\n",
						this_thread::get_id());

			done++;
		});

	// wait until the thread does done++
	done--;

	print("Hello from main tid={}\n", this_thread::get_id());
}

static void test_thread_creation_params(void)
{
	using namespace zpp;
	using namespace std::chrono;

	const thread_attr attr(
				thread_prio::preempt(0),
				thread_inherit_perms::no,
				thread_essential::no,
				thread_suspend::no
			);

	struct S {
		S() {
			print("S() {} {} {}",
				(void*)this, a, b);
		}

		S(const S& other) : a(other.a), b(other.b) {
			print("S(&{} {} {}) {} {} {}",
				(void*)&other, other.a, other.b,
				(void*)this, a, b);
		}
		~S() {
			print("~S() {} {} {}",
				(void*)this, a, b);
		}

		int a{};
		int b{};
	};

	sem done;

	S s;
	int a = 12;
	int b = 34;

	auto t = thread(
		tcb, attr,
		[&done](S s, int a, int b) {
			print("Hello from thread tid={} s.a={} s.b={} a={} b={}\n",
						this_thread::get_id(),
						s.a, s.b, a, b);

			s.a = a;
			s.b = b;

			done++;
		}, s, a, b);

	// wait until the thread does done++
	done--;

	print("Hello from main tid={}\n", this_thread::get_id());
}

void test_main(void)
{
	ztest_test_suite(zpp_thread_tests,
			 ztest_unit_test(test_thread_creation),
			 ztest_unit_test(test_thread_creation_params)
		);

	ztest_run_test_suite(zpp_thread_tests);
}
