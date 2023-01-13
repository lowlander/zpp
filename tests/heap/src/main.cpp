//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <zephyr/ztest.h>

#include <zephyr/kernel.h>

#include <zpp/heap.hpp>
#include <zpp/fmt.hpp>


ZTEST_SUITE(zpp_heap_tests, NULL, NULL, NULL, NULL, NULL);

namespace {

zpp::heap<1024> g_heap;

} // namespace

ZTEST(zpp_heap_tests, test_heap)
{
  auto p = g_heap.allocate(1);

  zassert_not_null(p, "");

  g_heap.deallocate(p);

  p = g_heap.allocate(128);

  zassert_not_null(p, "");

  g_heap.deallocate(p);
}
