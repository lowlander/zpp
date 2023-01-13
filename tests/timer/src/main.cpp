/*
* Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr/ztest.h>

#include <zephyr/kernel.h>

#include <zpp/timer.hpp>
#include <zpp/thread.hpp>
#include <zpp/fmt.hpp>

ZTEST_SUITE(zpp_timer_tests, NULL, NULL, NULL, NULL, NULL);

namespace {

auto g_t = zpp::make_timer();

void timer_callback(zpp::timer_base* t) noexcept
{
  zpp::print("Hello from timer tid={}\n", zpp::this_thread::get_id());
}

} // namespace

ZTEST(zpp_timer_tests, test_timer_creation_function)
{
  using namespace zpp;
  using namespace std::chrono;

  auto t = make_timer(timer_callback);
  t.start(100ms, 1s);

  this_thread::sleep_for(5s);
}

ZTEST(zpp_timer_tests, test_timer_creation_lambda)
{
  using namespace zpp;
  using namespace std::chrono;

  auto t = make_timer(
    [] (auto t) {
      print("Hello from timer tid={}\n",
          this_thread::get_id());
    } );

  t.start(100ms, 1s);

  this_thread::sleep_for(5s);
}
