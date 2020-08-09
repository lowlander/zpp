//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/fifo.hpp>
#include <zpp/thread.hpp>
#include <zpp/sem.hpp>

#include <array>

namespace {

zpp::thread_data<1024> tcb;

class item : public zpp::fifo_item_base {
public:
	uint32_t data{};
	uint32_t more_data{};
};

std::array<item, 4> g_item_array;

zpp::fifo<item> g_fifo;

void test_fifo()
{
	using namespace zpp;
	using namespace std::chrono;

	const thread_attr attr(
				thread_prio::preempt(0),
				thread_inherit_perms::yes,
				thread_essential::no,
				thread_suspend::no
			);

	sem end_sema(0, 1);

	//
	// Put items into fifo
	//
	for (auto& item: g_item_array) {
		item.data = 0x1234;
		item.more_data = 0x5678;
		g_fifo.push_back(&item);
	}

	auto t = thread(
		tcb, attr,
		[&end_sema]() {
			//
			// Get items from fifo
			//
			for (auto& item: g_item_array) {
				auto res = g_fifo.try_pop_front();
				zassert_equal(res->data, 0x1234, nullptr);
				zassert_equal(res->more_data, 0x5678, nullptr);
				zassert_equal(res, &item, nullptr);
			}

			//
			// Put items into fifo
			//
			for (auto& item: g_item_array) {
				g_fifo.push_back(&item);
			}

			end_sema++;
		});

	//
	// Let the child thread run
	//
	end_sema--;

	//
	// Get items from fifo
	//
	for (auto& item:  g_item_array) {
		auto res = g_fifo.try_pop_front();
		zassert_equal(res->data, 0x1234, nullptr);
		zassert_equal(res->more_data, 0x5678, nullptr);
		zassert_equal(res, &item, nullptr);
	}
}

} // namespace

void test_main(void)
{
	ztest_test_suite(test_zpp_fifo,
			 ztest_unit_test(test_fifo)
			);
	ztest_run_test_suite(test_zpp_fifo);
}
