/*
 * Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Simple test to check if all zpp headers are error free
 */

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp.hpp>

//
// check compile time power_of_two function
//
static_assert(zpp::power_of_two(0) == 1);
static_assert(zpp::power_of_two(1) == 2);
static_assert(zpp::power_of_two(2) == 4);
static_assert(zpp::power_of_two(3) == 8);
static_assert(zpp::power_of_two(4) == 16);
static_assert(zpp::power_of_two(10) == 1024);
static_assert(zpp::power_of_two(20) == 1048576);

//
// check compile time is_power_of_two function
//
static_assert(zpp::is_power_of_two(1) == true);
static_assert(zpp::is_power_of_two(2) == true);
static_assert(zpp::is_power_of_two(4) == true);
static_assert(zpp::is_power_of_two(8) == true);
static_assert(zpp::is_power_of_two(16) == true);
static_assert(zpp::is_power_of_two(3) == false);
static_assert(zpp::is_power_of_two(100) == false);

//
// check compile time is_multiple_of function
//
static_assert(zpp::is_multiple_of(0, 0) == false);
static_assert(zpp::is_multiple_of(4, 4) == true);
static_assert(zpp::is_multiple_of(6, 3) == true);
static_assert(zpp::is_multiple_of(10, 3) == false);

void test_main(void)
{
  /* This is only a compile time test */
}
