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

zpp::thread_data<STACK_SIZE> tcb[NUM_THREADS];
zpp::thread t[NUM_THREADS];

int done{};

void worker_thread(int myid)
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

  cv.notify_one();
}

} // anonimouse namespace

void main(void)
{
  const zpp::thread_attr attrs(
        zpp::thread_prio::preempt(10),
        zpp::thread_inherit_perms::no,
        zpp::thread_suspend::no
      );

  for (int i = 0; i < NUM_THREADS; i++) {
    t[i] = zpp::thread(tcb[0], attrs, worker_thread, i);
  }

  zpp::this_thread::sleep_for(std::chrono::seconds(1));

  zpp::unique_lock lk(m);

  //
  // are the other threads still busy?
  //
  while (done < NUM_THREADS) {
    printk("[thread %s] done is %d which is < %d so waiting on cond\n",
          __func__, done, (int)NUM_THREADS);

    // block this thread until another thread signals cond. While
    // blocked, the mutex is released, then re-acquired before this
    // thread is woken up and the call returns.
    cv.wait(m);

    printk("[thread %s] wake - cond was signalled.\n", __func__);

    // we go around the loop with the lock held
  }

  printk("[thread %s] done == %d so everyone is done\n",
    __func__, (int)NUM_THREADS);
}
