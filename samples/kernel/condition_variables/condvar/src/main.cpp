///
/// Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
/// Copyright (c) 2020 Intel Corporation
///
/// SPDX-License-Identifier: Apache-2.0
///

#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/sys/arch_interface.h>

#include <zpp.hpp>
#include <chrono>

#define NUM_THREADS 3
#define TCOUNT 10
#define COUNT_LIMIT 12

#define STACK_SIZE (1024 + CONFIG_TEST_EXTRA_STACK_SIZE)

namespace {

int count{};

zpp::mutex                count_mutex;
zpp::condition_variable   count_threshold_cv;

ZPP_THREAD_STACK_ARRAY_DEFINE(tstack, NUM_THREADS, STACK_SIZE);
zpp::thread_data tcb[NUM_THREADS];
zpp::thread t[NUM_THREADS];

} // anonimouse namespace

void inc_count(int my_id) noexcept
{
  for (int i = 0; i < TCOUNT; i++) {
    {
      zpp::lock_guard<zpp::mutex> lg(count_mutex);
      count++;

      //
      // Check the value of count and signal waiting thread when
      // condition is reached.  Note that this occurs while mutex is
      // locked.
      //

      if (count == COUNT_LIMIT) {
        printk("%s: thread %d, count = %d  Threshold reached.",
                                                      __func__, my_id, count);
        count_threshold_cv.notify_one();
        printk("Just sent signal.\n");
      }

      printk("%s: thread %d, count = %d, unlocking mutex\n",
                                                      __func__, my_id, count);
    }

    // Sleep so threads can alternate on mutex lock
    zpp::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

void watch_count(int my_id) noexcept
{
  printk("Starting %s: thread %d\n", __func__, my_id);

  zpp::unique_lock lk(count_mutex);

  while (count < COUNT_LIMIT) {
    printk("%s: thread %d Count= %d. Going into wait...\n",
          __func__, my_id, count);

    count_threshold_cv.wait(lk);

    printk("%s: thread %d Condition signal received. Count= %d\n",
          __func__, my_id, count);
  }

  printk("%s: thread %d Updating the value of count...\n",
        __func__, my_id);
  count += 125;
  printk("%s: thread %d count now = %d.\n", __func__, my_id, count);
  printk("%s: thread %d Unlocking mutex.\n", __func__, my_id);
}

int main(void)
{
  const zpp::thread_attr attrs(
        zpp::thread_prio::preempt(10),
        zpp::thread_inherit_perms::no,
        zpp::thread_suspend::no
      );

  t[0] = zpp::thread(tcb[0], tstack(0), attrs, watch_count, 1);

  t[1] = zpp::thread(tcb[1], tstack(1), attrs, inc_count, 2);
  t[2] = zpp::thread(tcb[2], tstack(2), attrs, inc_count, 3);

  // Wait for all threads to complete
  for (int i = 0; i < NUM_THREADS; i++) {
    t[i].join();
  }

  printk("Main(): Waited and joined with %d threads. Final value of count = %d. Done.\n",
        NUM_THREADS, count);

  return 0;
}
