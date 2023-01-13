/*
* Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr/ztest.h>

#include <zephyr/kernel.h>

#include <zpp/atomic_bitset.hpp>
#include <zpp/atomic_var.hpp>


ZTEST_SUITE(zpp_atomic_tests, NULL, NULL, NULL, NULL, NULL);

namespace {

zpp::atomic_bitset<320> g_bitset;

} // namespace

ZTEST(zpp_atomic_tests, test_atomic_bitset)
{
  g_bitset.store(0, true);
  zassert_true(g_bitset.load(0) == true, "load(0) failed");

  g_bitset.store(319, true);
  zassert_true(g_bitset.load(319) == true, "load(319) failed");
}
