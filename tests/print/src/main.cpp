/*
* Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
*
* SPDX-License-Identifier: Apache-2.0
*/

#include <zephyr/ztest.h>

#include <zpp/fmt.hpp>

ZTEST_SUITE(zpp_print_tests, NULL, NULL, NULL, NULL, NULL);

ZTEST(zpp_print_tests, test_print_uint8_t)
{
  uint8_t v{ 12 };
  zpp::print("uint8_t {} == 12\n", v);
}

ZTEST(zpp_print_tests, test_print_int8_t)
{
  int8_t v{ -12 };
  zpp::print("int8_t {} == -12\n", v);
}

ZTEST(zpp_print_tests, test_print_uint16_t)
{
  uint16_t v{ 1234 };
  zpp::print("uint16_t {} == 1234\n", v);
}

ZTEST(zpp_print_tests, test_print_int16_t)
{
  int16_t v{ -1234 };
  zpp::print("int16_t {} == -1234\n", v);
}

ZTEST(zpp_print_tests, test_print_uint32_t)
{
  uint32_t v{ 12345678 };
  zpp::print("uint32_t {} == 12345678\n", v);
}

ZTEST(zpp_print_tests, test_print_int32_t)
{
  int32_t v{ -12345678 };
  zpp::print("int32_t {} == -12345678\n", v);
}

ZTEST(zpp_print_tests, test_print_uint64_t)
{
  uint64_t v{ 12345678901011 };
  zpp::print("uint64_t {} == 12345678901011\n", v);
}

ZTEST(zpp_print_tests, test_print_int64_t)
{
  int64_t v{ -12345678901011 };
  zpp::print("int64_t {} == -12345678901011\n", v);
}

ZTEST(zpp_print_tests, test_print_char)
{
  char v{ 'c' };
  zpp::print("char {} == c\n", v);
}

ZTEST(zpp_print_tests, test_print_string)
{
  const char* v{ "string" };
  zpp::print("const char* {} == string\n", v);
}

ZTEST(zpp_print_tests, test_print_void_ptr)
{
  void* v{ (void*)0x12345678 };
  zpp::print("void* {} == 0x12345678\n", v);
}
