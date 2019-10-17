//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/mem_slab.hpp>
#include <zpp/fmt.hpp>

namespace {

ZPP_MEM_SLAB_DEFINE(g_mem_slab, int, 1, 1024);

void test_mem_slab()
{
	const auto total = g_mem_slab.total_block_count();

	zassert_equal(g_mem_slab.free_block_count(), total, "");
	zassert_equal(g_mem_slab.used_block_count(), 0, "");

	auto p = g_mem_slab.allocate(1);

	zassert_not_null(p, "");

	zassert_equal(g_mem_slab.free_block_count(), total - 1, "");
	zassert_equal(g_mem_slab.used_block_count(), 1, "");

	g_mem_slab.deallocate(p, 1);

	zassert_equal(g_mem_slab.free_block_count(), total, "");
	zassert_equal(g_mem_slab.used_block_count(), 0, "");
}

} // namespace

void test_main()
{
	ztest_test_suite(zpp_mem_slab_tests,
			 ztest_unit_test(test_mem_slab)
		);

	ztest_run_test_suite(zpp_mem_slab_tests);
}
