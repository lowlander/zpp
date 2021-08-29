/*
 * Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>
#include <sys/__assert.h>

#include<string.h>

#include <zpp.hpp>

namespace {

zpp::thread_data<1024> tcb;

bool ready = false;
bool processed = false;

char data[128] = {};

zpp::mutex m;

K_MUTEX_DEFINE(g_mutex);

zpp::mutex_ref m_ref(&g_mutex);

zpp::condition_variable cv;

K_CONDVAR_DEFINE(g_condvar);

zpp::condition_variable_ref cv_ref(&g_condvar);

void test_condition_variable_cmp()
{
  bool res;

  res = cv == cv_ref;
  zassert_false(res, "unable to compare condition_variable == condition_variable_ref\n");

  res = cv != cv_ref;
  zassert_true(res, "unable to compare condition_variable != condition_variable_ref\n");

  res = cv_ref == &g_condvar;
  zassert_true(res, "unable to compare condition_variable_ref == k_condvar*\n");

  res = cv_ref != &g_condvar;
  zassert_false(res, "unable to compare condition_variable_ref != k_condvar*\n");

  res = cv == &g_condvar;
  zassert_false(res, "unable to compare condition_variable == k_condvar*\n");

  res = cv != &g_condvar;
  zassert_true(res, "unable to compare condition_variable != k_condvar*\n");
}

void test_condition_variable()
{
  using namespace zpp;
  using namespace std::chrono;

  const thread_attr attr(
        thread_prio::preempt(0),
        thread_inherit_perms::no,
        thread_essential::no,
        thread_suspend::no
      );

  auto t = thread(
    tcb, attr,
    []() {
      // Wait until main() sends data
      zpp::unique_lock<zpp::mutex> lk(m);

      auto rc = cv.wait(lk, []{return ready;});
      __ASSERT_NO_MSG(rc == true);

      // after the wait, we own the lock.
      print("Worker thread is processing data\n");

      strcat(data, " after processing");

      // Send data back to main()
      processed = true;
      print("Worker thread signals data processing completed\n");

      // Manual unlocking is done before notifying, to avoid waking up
      // the waiting thread only to block again (see notify_one for details)
      rc = lk.unlock();
      __ASSERT_NO_MSG(rc == true);
      rc = cv.notify_one();
      __ASSERT_NO_MSG(rc == true);
    });

    strcpy(data, "Example data");
    // send data to the worker thread
    {
      zpp::lock_guard<zpp::mutex> lk(m);
      ready = true;
      print("main() signals data ready for processing\n");
    }
    auto rc = cv.notify_one();
    __ASSERT_NO_MSG(rc == true);

    // wait for the worker
    {
      zpp::unique_lock<zpp::mutex> lk(m);
      rc = cv.wait(lk, []{return processed;});
      __ASSERT_NO_MSG(rc == true);
    }
    print("Back in main(), data = {}\n", data);

    rc = t.join();
    __ASSERT_NO_MSG(rc == true);
}

} // namespace

void test_main(void)
{
  ztest_test_suite(test_zpp_condition_variable,
    ztest_unit_test(test_condition_variable_cmp),
    ztest_unit_test(test_condition_variable)
  );
  ztest_run_test_suite(test_zpp_condition_variable);
}