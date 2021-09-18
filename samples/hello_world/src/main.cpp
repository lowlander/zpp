//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <zpp.hpp>

#include <chrono>

//
// using a anonymous namespace for file local variables and functions
//
namespace {

//
// Define the Thread Control Block for the thread, using a stack of 1024 bytes
//
ZPP_THREAD_STACK_DEFINE(my_thread_tstack, 1024);
zpp::thread_data my_thread_tcb;

zpp::heap<128> my_heap;

} // namespace

int main(int argc, char *argv[])
{
  //
  // use zpp and std::chrono namespaces in the function
  //
  using namespace zpp;
  using namespace std::chrono;

  mutex print_lock;

  //
  // Create thread attributes used for thread creation
  //
  const thread_attr my_thread_attr(
        thread_prio::preempt(0),
        thread_inherit_perms::no,
        thread_suspend::no
      );

  //
  // Create the first thread, using a lambda passing a const char* as
  // argument that is used as "template" for the print out.
  //
  // The lock used for making sure the prints don't get messed up
  // is captured by reference
  //
  const char* string_arg = "Hello World from thread tid={}\n";

  auto my_thread = thread(
    my_thread_tcb, my_thread_tstack(), my_thread_attr, &my_heap,
    [&print_lock](const char* t) noexcept
    {
      while (true) {
        //
        // Use a lock_guard in a scope to automatically
        // do the unlocking when leaving the scope
        //
        {
          lock_guard lg(print_lock);

          //
          // print the thread ID of this thread
          //
          print(t, this_thread::get_id());
        }

        //
        // let this thread sleep for 500 ms without
        // holding the lock
        //
        this_thread::sleep_for(500ms);
      }
    }, string_arg);

  //
  // Loop forever, because ending main would not only end the
  // program, my_thread would also go out of scope aborting the thread.
  //
  while (true) {
    // Use a lock_guard in a scope to automatically
    // do the unlocking when leaving the scope
    //
    {
      lock_guard lg(print_lock);

      //
      // print the thread ID of the main thread
      //
      print("Hello World from main tid={}\n",
              this_thread::get_id());
    }

    //
    // let main thread sleep for 1 s without holding the lock
    //
    this_thread::sleep_for(1s);
  }

  return 0;
}
