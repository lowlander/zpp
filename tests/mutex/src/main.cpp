/*
* Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr/ztest.h>

#include <zephyr/kernel.h>

#include <zpp/mutex.hpp>
#include <zpp/thread.hpp>
#include <zpp/lock_guard.hpp>
#include <zpp/utils.hpp>


ZTEST_SUITE(test_zpp_mutex, NULL, NULL, NULL, NULL, NULL);

namespace {

zpp::mutex m;

K_MUTEX_DEFINE(g_mutex);

zpp::mutex_ref m_ref(&g_mutex);

} // namespace

ZTEST(test_zpp_mutex, test_mutex_cmp)
{
  bool res;

  res = m == m_ref;
  zassert_false(res, "unable to compare mutex == mutex_ref\n");

  res = m != m_ref;
  zassert_true(res, "unable to compare mutex != mutex_ref\n");

  res = m_ref == m;
  zassert_false(res, "unable to compare mutex_ref == mutex\n");

  res = m_ref != m;
  zassert_true(res, "unable to compare mutex_ref != mutex\n");

  res = m_ref == &g_mutex;
  zassert_true(res, "unable to compare mutex_ref == k_condvar*\n");

  res = m_ref != &g_mutex;
  zassert_false(res, "unable to compare mutex_ref != k_condvar*\n");

  res = m == &g_mutex;
  zassert_false(res, "unable to compare mutex == k_condvar*\n");

  res = m != &g_mutex;
  zassert_true(res, "unable to compare mutex != k_mutex*\n");

  res =  &g_mutex == m_ref;
  zassert_true(res, "unable to compare k_condvar* == mutex_ref\n");

  res = &g_mutex != m_ref;
  zassert_false(res, "unable to compare k_condvar* != mutex_ref\n");

  res = &g_mutex == m;
  zassert_false(res, "unable to compare mutex == k_condvar* == mutex\n");

  res = &g_mutex != m;
  zassert_true(res, "unable to compare k_mutex* != mutex\n");

}

ZTEST(test_zpp_mutex, test_mutex)
{
  auto rc = m.lock();

  zassert_true(!!rc, "Failed to lock mutex: %d\n", rc.error());

  rc = m.unlock();

  zassert_true(!!rc, "Failed to unlock mutex: %d\n", rc.error());
}

ZTEST(test_zpp_mutex, test_mutex_ref)
{
  auto rc = m_ref.lock();

  zassert_true(!!rc, "Failed to lock mutex_ref: %d\n", rc.error());

  rc = m_ref.unlock();

  zassert_true(!!rc, "Failed to unlock mutex_ref: %d\n", rc.error());
}

ZTEST(test_zpp_mutex, test_lock_guard)
{
  zpp::lock_guard g(m);
  zpp::lock_guard g_ref(m_ref);
}
