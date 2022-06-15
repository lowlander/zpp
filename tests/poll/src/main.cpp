//
// Copyright (c) 2017 Wind River Systems, Inc.
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <ztest.h>

#include <zephyr/kernel.h>

#include <zpp/poll.hpp>
#include <zpp/sem.hpp>
#include <zpp/fifo.hpp>


ZTEST_SUITE(zpp_poll_tests, NULL, NULL, NULL, NULL, NULL);

namespace {

#define SIGNAL_RESULT 0x1ee7d00d
#define FIFO_MSG_VALUE 0xdeadbeef

ZPP_THREAD_STACK_DEFINE(test_thread_stack, 1024);
zpp::thread_data test_thread;


struct fifo_msg {
  void* fifo_reserved{};
  uint32_t msg { FIFO_MSG_VALUE };
};

fifo_msg wait_msg;

zpp::sem            wait_sem;
zpp::fifo<fifo_msg> wait_fifo;
zpp::poll_signal    wait_signal;

zpp::poll_event_set wait_events {
  wait_sem,
  wait_fifo,
  wait_signal,
  nullptr
};

} // namespace

ZTEST(zpp_poll_tests, test_poll_wait)
{
  using namespace std::chrono;

  const zpp::thread_prio main_low_prio{ 10 };

  auto old_prio = zpp::this_thread::get_priority();

  //
  // Wait for 3 non-ready events to become ready from a higher priority
  // thread.
  //
  zpp::this_thread::set_priority(main_low_prio);

  zpp::thread_attr attr(
      main_low_prio + 1,
      zpp::thread_inherit_perms::yes,
      zpp::thread_essential::no,
      zpp::thread_suspend::no );

  auto poll_helper = [](bool do_fifo) noexcept {
      zpp::this_thread::sleep_for(250ms);

      wait_sem++;

      if (do_fifo) {
        wait_fifo.push_back(&wait_msg);
      }

      wait_signal.raise(SIGNAL_RESULT);
  };

  auto t = zpp::thread(test_thread, test_thread_stack(), attr, poll_helper, true);

  auto rc = wait_events.try_poll_for(1s);

  zpp::this_thread::set_priority(old_prio);

  zassert_equal(rc, true, "");

  zassert_equal(wait_events[0].is_ready(), true, "");
  zassert_equal(wait_sem.try_take(), true, "");

  zassert_equal(wait_events[1].is_ready(), true, "");
  auto msg_ptr = wait_fifo.try_pop_front();
  zassert_not_null(msg_ptr, "");
  zassert_equal(msg_ptr, &wait_msg, "");
  zassert_equal(msg_ptr->msg, FIFO_MSG_VALUE, "");

  zassert_equal(wait_events[2].is_ready(), true, "");
  zassert_equal(wait_signal.check().value_or(-1), SIGNAL_RESULT, "");

  zassert_equal(wait_events[3].is_ready(), false, "");

  //
  // verify events are not ready anymore
  //
  wait_events[0].reset();
  wait_events[1].reset();
  wait_events[2].reset();
  wait_events[3].reset();
  wait_signal.reset();

  zassert_equal(wait_events.try_poll_for(1s), false, "");

  zassert_equal(wait_events[0].is_ready(), false, "");
  zassert_equal(wait_events[1].is_ready(), false, "");
  zassert_equal(wait_events[2].is_ready(), false, "");
  zassert_equal(wait_events[3].is_ready(), false, "");

  //
  // Wait for 2 out of 3 non-ready events to become ready from a higher
  // priority thread.
  //
  zpp::this_thread::set_priority(main_low_prio);

  attr.set(old_prio + 1);
  t = zpp::thread(test_thread, test_thread_stack(), attr, poll_helper, false);

  rc = wait_events.try_poll_for(1s);

  zpp::this_thread::set_priority(old_prio);

  zassert_equal(rc, true, "");

  zassert_equal(wait_events[0].is_ready(), true, "");
  zassert_equal(wait_sem.try_take(), true, "");

  zassert_equal(wait_events[1].is_ready(), false, "");
  msg_ptr = wait_fifo.try_pop_front();
  zassert_is_null(msg_ptr, "");

  zassert_equal(wait_events[2].is_ready(), true, "");
  zassert_equal(wait_signal.check().value_or(-1), SIGNAL_RESULT, "");

  zassert_equal(wait_events[3].is_ready(), false, "");


  //
  // Wait for each event to be ready from a lower priority thread, one at
  // a time.
  //
  wait_events[0].reset();
  wait_events[1].reset();
  wait_events[2].reset();
  wait_events[3].reset();
  wait_signal.reset();

  attr.set(old_prio - 1);
  t = zpp::thread(test_thread, test_thread_stack(), attr, poll_helper, true);

  //
  // semaphore
  //
  rc = wait_events.try_poll_for(1s);

  zassert_equal(rc, true, "");

  zassert_equal(wait_events[0].is_ready(), true, "");
  zassert_equal(wait_sem.try_take(), true, "");

  zassert_equal(wait_events[1].is_ready(), false, "");
  msg_ptr = wait_fifo.try_pop_front();
  zassert_is_null(msg_ptr, "");

  zassert_equal(wait_events[2].is_ready(), false, "");

  wait_events[0].reset();

  //
  // fifo
  //
  rc = wait_events.try_poll_for(1s);

  zassert_equal(rc, true, "");

  zassert_equal(wait_events[0].is_ready(), false, "");
  zassert_equal(wait_sem.try_take(), false, "");

  zassert_equal(wait_events[1].is_ready(), true, "");
  msg_ptr = wait_fifo.try_pop_front();
  zassert_not_null(msg_ptr, "");

  zassert_equal(wait_events[2].is_ready(), false, "");

  wait_events[1].reset();

  //
  // poll signal
  //
  rc = wait_events.try_poll_for(1s);

  zassert_equal(rc, true, "");

  zassert_equal(wait_events[0].is_ready(), false, "");
  zassert_equal(wait_sem.try_take(), false, "");

  zassert_equal(wait_events[1].is_ready(), false, "");
  msg_ptr = wait_fifo.try_pop_front();
  zassert_is_null(msg_ptr, "");

  zassert_equal(wait_events[2].is_ready(), true, "");
  zassert_equal(wait_signal.check().value_or(-1), SIGNAL_RESULT, "");

  wait_events[2].reset();
  wait_signal.reset();
}
