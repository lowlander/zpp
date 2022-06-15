/*
* Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <ztest.h>

#include <zpp/thread.hpp>
#include <zpp/sem.hpp>
#include <zpp/fmt.hpp>

#include <chrono>


ZTEST_SUITE(zpp_thread_tests, NULL, NULL, NULL, NULL, NULL);

namespace {

ZPP_THREAD_STACK_DEFINE(tstack, 1024);
zpp::thread_data tcb;

zpp::heap<1024> theap;

} // namespace

ZTEST(zpp_thread_tests, test_thread_creation)
{
  using namespace zpp;
  using namespace std::chrono;

  const thread_attr attr(
        thread_prio::preempt(0),
        thread_inherit_perms::no,
        thread_essential::no,
        thread_suspend::no
      );

  int dummy=42;
  sem done;

  auto t = thread(
    tcb, tstack(), attr, &theap,
    [&dummy, &done]() noexcept {
      print("Hello from thread tid={}\n",
            this_thread::get_id());

      print("dummy = {}\n", (void*)&dummy);
      print("done = {}\n", (void*)&done);

      zassert_true(dummy == 42, "dummy not 42\n");

      done++;
    });

  // wait until the thread does done++
  done--;

  auto  rc = t.join();
  zassert_true(rc == true, "join failed");

  print("Hello from main tid={}\n", this_thread::get_id());
}

ZTEST(zpp_thread_tests, test_thread_creation_void)
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
    tcb, tstack(), attr,
    []() noexcept {
      print("Hello from thread tid={}\n",
            this_thread::get_id());
    });

  auto rc = t.join();
  zassert_true(rc == true, "join failed");

  print("Hello from main tid={}\n", this_thread::get_id());
}

ZTEST(zpp_thread_tests, test_thread_creation_pointer)
{
  using namespace zpp;
  using namespace std::chrono;

  const thread_attr attr(
        thread_prio::preempt(0),
        thread_inherit_perms::no,
        thread_essential::no,
        thread_suspend::no
      );

  struct S {
    S() noexcept {
      print("S() {} {} {}",
        (void*)this, a, b);
    }

    S(const S& other) noexcept : a(other.a), b(other.b)  {
      print("S(&{} {} {}) {} {} {}",
        (void*)&other, other.a, other.b,
        (void*)this, a, b);
    }
    ~S() noexcept {
      print("~S() {} {} {}",
        (void*)this, a, b);
    }

    int a{};
    int b{};
  };

  S s;

  auto t = thread(
    tcb, tstack(), attr,
    [](S* s) noexcept {
      print("Hello from thread tid={} s->a={} s->b={}\n",
            this_thread::get_id(),
            s->a, s->b);

      s->a = 21;
      s->b = 43;

    }, &s);

  auto rc = t.join();
  zassert_true(rc == true, "join failed");

  zassert_true(s.a == 21, "s.a != 21\n");
  zassert_true(s.b == 43, "s.a != 43\n");

  print("Hello from main tid={}\n", this_thread::get_id());
}


ZTEST(zpp_thread_tests, test_thread_creation_params)
{
  using namespace zpp;
  using namespace std::chrono;

  const thread_attr attr(
        thread_prio::preempt(0),
        thread_inherit_perms::no,
        thread_essential::no,
        thread_suspend::no
      );

  struct S {
    S() noexcept {
      print("S() {} {} {}",
        (void*)this, a, b);
    }

    S(const S& other) noexcept : a(other.a), b(other.b)  {
      print("S(&{} {} {}) {} {} {}",
        (void*)&other, other.a, other.b,
        (void*)this, a, b);
    }
    ~S() noexcept {
      print("~S() {} {} {}",
        (void*)this, a, b);
    }

    int a{};
    int b{};
  };

  sem done;

  S s;
  int a = 12;
  int b = 34;

  auto t = thread(
    tcb, tstack(), attr, &theap,
    [&done](S s, int a, int b) noexcept {
      print("Hello from thread tid={} s.a={} s.b={} a={} b={}\n",
            this_thread::get_id(),
            s.a, s.b, a, b);

      s.a = a;
      s.b = b;

      done++;
    }, s, a, b);

  // wait until the thread does done++
  done--;

  auto rc = t.join();
  zassert_true(rc == true, "join failed");

  print("Hello from main tid={}\n", this_thread::get_id());
}
