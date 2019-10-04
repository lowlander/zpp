/*
 * Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ztest.h>

#include <zpp/fmt.hpp>

namespace {

void test_print_u8_t(void)
{
	u8_t v{ 12 };
	zpp::print("u8_t {} == 12\n", v);
}

void test_print_s8_t(void)
{
	s8_t v{ -12 };
	zpp::print("s8_t {} == -12\n", v);
}

void test_print_u16_t(void)
{
	u16_t v{ 1234 };
	zpp::print("u16_t {} == 1234\n", v);
}

void test_print_s16_t(void)
{
	s16_t v{ -1234 };
	zpp::print("s16_t {} == -1234\n", v);
}

void test_print_u32_t(void)
{
	u32_t v{ 12345678 };
	zpp::print("u32_t {} == 12345678\n", v);
}

void test_print_s32_t(void)
{
	s32_t v{ -12345678 };
	zpp::print("s32_t {} == -12345678\n", v);
}

void test_print_u64_t(void)
{
	u64_t v{ 12345678901011 };
	zpp::print("u64_t {} == 12345678901011\n", v);
}

void test_print_s64_t(void)
{
	s64_t v{ -12345678901011 };
	zpp::print("s64_t {} == -12345678901011\n", v);
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
	TC_PRINT("C++ version %u\n", (u32_t)__cplusplus);

	ztest_test_suite(zpp_print_tests,
			 ztest_unit_test(test_print_u8_t),
			 ztest_unit_test(test_print_s8_t),
			 ztest_unit_test(test_print_u16_t),
			 ztest_unit_test(test_print_s16_t),
			 ztest_unit_test(test_print_u32_t),
			 ztest_unit_test(test_print_s32_t),
			 ztest_unit_test(test_print_u64_t),
			 ztest_unit_test(test_print_s64_t),
			 ztest_unit_test(test_print_char),
			 ztest_unit_test(test_print_string),
			 ztest_unit_test(test_print_void_ptr)
		);

	ztest_run_test_suite(zpp_print_tests);
}
