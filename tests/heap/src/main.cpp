//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/heap.hpp>
#include <zpp/fmt.hpp>

namespace {

zpp::heap<1024> g_heap;

void test_heap()
{
  auto p = g_heap.allocate(1);

  zassert_not_null(p, "");

  g_heap.deallocate(p);

  p = g_heap.allocate(128);

  zassert_not_null(p, "");

  g_heap.deallocate(p);
}

} // namespace

void test_main()
{
  ztest_test_suite(zpp_heap_tests,
      ztest_unit_test(test_heap)
    );

  ztest_run_test_suite(zpp_heap_tests);
}
