//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/mem_pool.hpp>
#include <zpp/fmt.hpp>

namespace {

ZPP_MEM_POOL_DEFINE(g_mem_pool, int, 1, 128, 1024);

void test_mem_pool()
{
	auto p = g_mem_pool.allocate(1);

	zassert_not_null(p, "");

	g_mem_pool.deallocate(p, 1);

	p = g_mem_pool.allocate(128);

	zassert_not_null(p, "");

	g_mem_pool.deallocate(p, 128);
}

} // namespace

void test_main()
{
	ztest_test_suite(zpp_mem_pool_tests,
			 ztest_unit_test(test_mem_pool)
		);

	ztest_run_test_suite(zpp_mem_pool_tests);
}
