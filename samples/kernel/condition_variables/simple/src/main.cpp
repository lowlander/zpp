/*
* Copyright (c) 2020 Intel Corporation
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr.h>
#include <arch/cpu.h>
#include <sys/arch_interface.h>

#include <zpp.hpp>
#include <chrono>

#define NUM_THREADS 20
#define STACK_SIZE (1024 + CONFIG_TEST_EXTRA_STACKSIZE)

namespace {

zpp::mutex                m;
zpp::condition_variable   cv;


ZPP_THREAD_STACK_ARRAY_DEFINE(tstack, NUM_THREADS, STACK_SIZE);
zpp::thread_data tcb[NUM_THREADS];
zpp::thread t[NUM_THREADS];

int done{};

void worker_thread(int myid) noexcept
{
  const int workloops = 5;

  for (int i = 0; i < workloops; i++) {
    printk("[thread %d] working (%d/%d)\n", myid, i, workloops);

    zpp::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  //
  // we're going to manipulate done and use the cond, so we need the mutex
  //
  zpp::lock_guard lg(m);

  //
  // increase the count of threads that have finished their work.
  //
  done++;
  printk("[thread %d] done is now %d. Signalling cond.\n", myid, done);

  auto res = cv.notify_one();
  __ASSERT_NO_MSG(res == true);
}

} // anonimouse namespace

int main(void)
{
  const zpp::thread_attr attrs(
        zpp::thread_prio::preempt(10),
        zpp::thread_inherit_perms::no,
        zpp::thread_suspend::no
      );

  for (int i = 0; i < NUM_THREADS; i++) {
    t[i] = zpp::thread(tcb[i], tstack(i), attrs, &worker_thread, i);
  }

  //zpp::this_thread::sleep_for(std::chrono::seconds(1));

  printk("[thread %s] all threads started\n", __func__);

  {
    zpp::lock_guard lg(m);

  //
  // are the other threads still busy?
  //
  while (done < NUM_THREADS) {
    printk("[thread %s] done is %d which is < %d so waiting on cond\n",
          __func__, done, (int)NUM_THREADS);

    // block this thread until another thread signals cond. While
    // blocked, the mutex is released, then re-acquired before this
    // thread is woken up and the call returns.
    auto res = cv.wait(m);
    __ASSERT_NO_MSG(res == true);

    printk("[thread %s] wake - cond was signalled.\n", __func__);

    // we go around the loop with the lock held
  }

  } // zpp::lock_guard scope

  for (int i = 0; i < NUM_THREADS; i++) {
    auto res = t[i].join();
    __ASSERT_NO_MSG(res == true);
  }

  printk("[thread %s] done == %d so everyone is done\n",
    __func__, (int)NUM_THREADS);

  return 0;
}
