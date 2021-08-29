//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/mem_slab.hpp>

namespace {

zpp::mem_slab<8, 64, 8> g_mem_slab;

void test_mem_slab()
{
  const auto total = g_mem_slab.total_block_count();

  zassert_equal(g_mem_slab.free_block_count(), total, "");
  zassert_equal(g_mem_slab.used_block_count(), 0, "");

  auto p = g_mem_slab.allocate();

  zassert_not_null(p, "");

  zassert_equal(g_mem_slab.free_block_count(), total - 1, "");
  zassert_equal(g_mem_slab.used_block_count(), 1, "");

  g_mem_slab.deallocate(p);

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
