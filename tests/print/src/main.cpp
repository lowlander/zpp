/*
 * Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

#include <zpp/fmt.hpp>

namespace {

void test_print_uint8_t(void)
{
	uint8_t v{ 12 };
	zpp::print("uint8_t {} == 12\n", v);
}

void test_print_int8_t(void)
{
	int8_t v{ -12 };
	zpp::print("int8_t {} == -12\n", v);
}

void test_print_uint16_t(void)
{
	uint16_t v{ 1234 };
	zpp::print("uint16_t {} == 1234\n", v);
}

void test_print_int16_t(void)
{
	int16_t v{ -1234 };
	zpp::print("int16_t {} == -1234\n", v);
}

void test_print_uint32_t(void)
{
	uint32_t v{ 12345678 };
	zpp::print("uint32_t {} == 12345678\n", v);
}

void test_print_int32_t(void)
{
	int32_t v{ -12345678 };
	zpp::print("int32_t {} == -12345678\n", v);
}

void test_print_uint64_t(void)
{
	uint64_t v{ 12345678901011 };
	zpp::print("uint64_t {} == 12345678901011\n", v);
}

void test_print_int64_t(void)
{
	int64_t v{ -12345678901011 };
	zpp::print("int64_t {} == -12345678901011\n", v);
}

void test_print_char(void)
{
	char v{ 'c' };
	zpp::print("char {} == c\n", v);
}

void test_print_string(void)
{
	const char* v{ "string" };
	zpp::print("const char* {} == string\n", v);
}

void test_print_void_ptr(void)
{
	void* v{ (void*)0x12345678 };
	zpp::print("void* {} == 0x12345678\n", v);
}

} // namespace

void test_main(void)
{
	TC_PRINT("C++ version %u\n", (uint32_t)__cplusplus);

	ztest_test_suite(zpp_print_tests,
			 ztest_unit_test(test_print_uint8_t),
			 ztest_unit_test(test_print_int8_t),
			 ztest_unit_test(test_print_uint16_t),
			 ztest_unit_test(test_print_int16_t),
			 ztest_unit_test(test_print_uint32_t),
			 ztest_unit_test(test_print_int32_t),
			 ztest_unit_test(test_print_uint64_t),
			 ztest_unit_test(test_print_int64_t),
			 ztest_unit_test(test_print_char),
			 ztest_unit_test(test_print_string),
			 ztest_unit_test(test_print_void_ptr)
		);

	ztest_run_test_suite(zpp_print_tests);
}
