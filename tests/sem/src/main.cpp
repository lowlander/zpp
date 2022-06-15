/*
* Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <ztest.h>

#include <zephyr/kernel.h>

#include <zpp/sem.hpp>
#include <zpp/thread.hpp>
#include <zpp/utils.hpp>

ZTEST_SUITE(test_zpp_sem, NULL, NULL, NULL, NULL, NULL);

namespace {

K_SEM_DEFINE(g_sem, 0, 10);

zpp::sem_ref simple_ref_sem(&g_sem);

zpp::sem simple_sem(0, 10);

} // namespace

ZTEST(test_zpp_sem, test_sem_cmp)
{
  bool res;

  res = simple_sem == simple_ref_sem;
  zassert_false(res, "unable to compare sem == sem_ref\n");

  res = simple_sem != simple_ref_sem;
  zassert_true(res, "unable to compare sem != sem_ref\n");

  res = simple_ref_sem == simple_sem;
  zassert_false(res, "unable to compare sem_ref == sem\n");

  res = simple_ref_sem != simple_sem;
  zassert_true(res, "unable to compare sem_ref != sem\n");

  res = simple_ref_sem == &g_sem;
  zassert_true(res, "unable to compare sem_ref == k_sem*\n");

  res = simple_ref_sem != &g_sem;
  zassert_false(res, "unable to compare sem_ref != k_sem*\n");

  res = simple_sem == &g_sem;
  zassert_false(res, "unable to compare sem == k_sem*\n");

  res = simple_sem != &g_sem;
  zassert_true(res, "unable to compare sem != k_sem*\n");

  res =  &g_sem == simple_ref_sem;
  zassert_true(res, "unable to compare k_sem* == sem_ref\n");

  res = &g_sem != simple_ref_sem;
  zassert_false(res, "unable to compare k_sem* != sem_ref\n");

  res = &g_sem == simple_sem;
  zassert_false(res, "unable to compare k_sem* == sem\n");

  res = &g_sem != simple_sem;
  zassert_true(res, "unable to compare k_sem* != sem\n");

}

ZTEST(test_zpp_sem, test_sem_try_take)
{
  simple_sem.reset();

  for (int i = 0; i < 5; i++) {
    simple_sem++;

    auto signal_count = simple_sem.count();
    zassert_true(signal_count == (i + 1),
                 "signal count missmatch Expected %d, got %d\n",
                 (i + 1), signal_count);
  }

  for (int i = 4; i >= 0; i--) {
    auto ret_value = simple_sem.try_take();
    zassert_true(ret_value == true,
                 "unable to do k_sem_take which returned %d\n",
                 ret_value);

    auto signal_count = simple_sem.count();
    zassert_true(signal_count == i,
                 "signal count missmatch Expected %d, got %d\n",
                 i, signal_count);
  }
}

ZTEST(test_zpp_sem, test_sem_try_take_fails)
{
  simple_sem.reset();

  for (int i = 4; i >= 0; i--) {
    auto ret_value = simple_sem.try_take();
    zassert_true(ret_value == false,
                 "k_sem_take returned when not possible");

    auto signal_count = simple_sem.count();
    zassert_true(signal_count == 0U,
                 "signal count missmatch Expected 0, got %d\n",
                 signal_count);
  }
}

ZTEST(test_zpp_sem, test_sem_try_take_for_fails)
{
  using namespace std::chrono;

  simple_sem.reset();

  for (int i = 4; i >= 0; i--) {
    auto ret_value = simple_sem.try_take_for(100ms);
    zassert_true(ret_value == false,
                 "k_sem_take succeeded when its not possible");
  }
}

ZTEST(test_zpp_sem, test_sem_try_take_ref)
{
  simple_ref_sem.reset();

  for (int i = 0; i < 5; i++) {
    simple_ref_sem++;

    auto signal_count = simple_ref_sem.count();
    zassert_true(signal_count == (i + 1),
                 "signal count missmatch Expected %d, got %d\n",
                 (i + 1), signal_count);
  }

  for (int i = 4; i >= 0; i--) {
    auto ret_value = simple_ref_sem.try_take();
    zassert_true(ret_value == true,
                 "unable to do k_sem_take which returned %d\n",
                 ret_value);

    auto signal_count = simple_ref_sem.count();
    zassert_true(signal_count == i,
                 "signal count missmatch Expected %d, got %d\n",
                 i, signal_count);
  }
}

ZTEST(test_zpp_sem, test_sem_try_take_fails_ref)
{
  simple_ref_sem.reset();

  for (int i = 4; i >= 0; i--) {
    auto ret_value = simple_ref_sem.try_take();
    zassert_true(ret_value == false,
                 "k_sem_take returned when not possible");

    auto signal_count = simple_ref_sem.count();
    zassert_true(signal_count == 0U,
                 "signal count missmatch Expected 0, got %d\n",
                 signal_count);
  }
}

ZTEST(test_zpp_sem, test_sem_try_take_for_fails_ref)
{
  using namespace std::chrono;

  simple_ref_sem.reset();

  for (int i = 4; i >= 0; i--) {
    auto ret_value = simple_ref_sem.try_take_for(100ms);
    zassert_true(ret_value == false,
                 "k_sem_take succeeded when its not possible");
  }
}
