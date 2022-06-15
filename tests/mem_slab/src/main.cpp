//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <ztest.h>

#include <zephyr/kernel.h>

#include <zpp/mem_slab.hpp>


ZTEST_SUITE(zpp_mem_slab_tests, NULL, NULL, NULL, NULL, NULL);

namespace {

zpp::mem_slab<8, 64, 8> g_mem_slab;

} // namespace

ZTEST(zpp_mem_slab_tests, test_mem_slab)
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
