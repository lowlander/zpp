/*
 * Copyright (c) 2021 Erwin Rol <erwin@erwinrol.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>
#include <sys/__assert.h>

#include<string.h>

#include <zpp.hpp>

namespace {

class ResultData {
public:
  ResultData() = delete;
  ResultData(int v) noexcept : m_data(v) {}
private:
  int m_data{42};
};

class ErrorData {
public:
  ErrorData() = delete;
  ErrorData(int v) noexcept : m_data(v) {}
private:
  int m_data{42};
};


void test_result()
{
  zpp::result<int, zpp::error_code>   res_a;
  zpp::result<char, zpp::error_code>  res_b;
  zpp::result<void*, zpp::error_code> res_c;

  zassert_true(!res_a, "res_a should be false\n");
  zassert_false(res_b, "res_b should be false\n");
  zassert_true(res_c == false, "res_c should be false\n");

  res_a = zpp::error_result(zpp::error_code::k_inval);

  zassert_true(res_a == false, "res_a should be false\n");

  res_a = 12345;

  zassert_true(res_a == true, "res_a should be true\n");
  zassert_true(res_a.value() == 12345, "res_a.value() should be 12345\n");

  auto res_d = res_a;
  zassert_true(res_d == true, "res_d should be true\n");
  zassert_true(res_d.value() == 12345, "res_d.value() should be 12345\n");

  res_d.assign_error(zpp::error_code::k_already);
  zassert_false(res_d, "res_d should be false\n");
  zassert_true(res_d.error() == zpp::error_code::k_already, "res_d should hold zpp::error_code::k_already");

  zpp::result<void, int> res_e;
  zassert_false(res_e, "res_d should be true\n");

  res_e.assign_value();
  zassert_true(!!res_e, "res_d should be true\n");

  res_e = zpp::error_result(0);
  zassert_true(res_e.error() == 0, "res_d should hold 0");

  zpp::result<ResultData, zpp::error_code>   res_f;
  zassert_false(res_f, "res_f should be false\n");

  zpp::result<void, ErrorData>   res_g(zpp::error_result(ErrorData(13)));
  zassert_false(res_g, "res_g should be false\n");

  zpp::result<ResultData, ErrorData>   res_h(zpp::error_result(ErrorData(13)));
  zassert_false(res_h, "res_h should be false\n");

}

} // namespace

void test_main(void)
{
  ztest_test_suite(test_zpp_result,
    ztest_unit_test(test_result)
  );
  ztest_run_test_suite(test_zpp_result);
}
