/*
* Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/atomic_bitset.hpp>
#include <zpp/atomic_var.hpp>

namespace {

zpp::atomic_bitset<320> g_bitset;

void test_atomic_bitset()
{
  g_bitset.store(0, true);
  zassert_true(g_bitset.load(0) == true, "load(0) failed");

  g_bitset.store(319, true);
  zassert_true(g_bitset.load(319) == true, "load(319) failed");
}

} // namespace

void test_main()
{
  ztest_test_suite(zpp_atomic_tests,
      ztest_unit_test(test_atomic_bitset)
    );

  ztest_run_test_suite(zpp_atomic_tests);
}
