//
// Copyright (c) 2019 Erwin Rol <erwin@erwinrol.com>
//
// SPDX-License-Identifier: Apache-2.0
//

#include <zephyr.h>
#include <kernel.h>
#include <ztest.h>

#include <zpp/mem_slab.hpp>
#include <zpp/fmt.hpp>

namespace {

int g_constructor_count {0};
int g_destructor_count {0};

class item {
public:
	item() noexcept
	{
		g_constructor_count++;
	}

	item(int a, int b) noexcept
		: m_a(a), m_b(b)
	{
		g_constructor_count++;
	}

	item(const item& other) noexcept
		: m_a(other.m_a), m_b(other.m_b)
	{
		g_constructor_count++;
	}

	item(item&& other) noexcept
		: m_a(std::move(other.m_a)), m_b(std::move(other.m_b))
	{
		g_constructor_count++;
	}

	~item() noexcept {
		g_destructor_count++;
	}
public:
	int	m_a{ 1234 };
	int	m_b{ 4321 };
};

zpp::mem_slab<item, 64> g_mem_slab;

void test_mem_slab()
{
	zassert_equal(g_mem_slab.total_block_count(), 64, "");
	zassert_equal(g_mem_slab.free_block_count(), 64, "");
	zassert_equal(g_mem_slab.used_block_count(), 0, "");

	zassert_equal(g_constructor_count, 0, "");
	zassert_equal(g_destructor_count, 0, "");

	auto p = g_mem_slab.try_alloc();

	zassert_equal(g_mem_slab.free_block_count(), 63, "");
	zassert_equal(g_mem_slab.used_block_count(), 1, "");

	zassert_equal(g_constructor_count, 1, "");
	zassert_equal(g_destructor_count, 0, "");

	zassert_not_null(p, "");

	zassert_equal(p->m_a, 1234, "");
	zassert_equal(p->m_b, 4321, "");

	p->m_a = 5678;
	p->m_b = 8765;

	zassert_equal((*p).m_a, 5678, "");
	zassert_equal((*p).m_b, 8765, "");

	p = g_mem_slab.try_alloc(123456789, 987654321);

	zassert_equal(g_mem_slab.free_block_count(), 63, "");
	zassert_equal(g_mem_slab.used_block_count(), 1, "");

	zassert_equal(g_constructor_count, 2, "");
	zassert_equal(g_destructor_count, 1, "");

	zassert_not_null(p, "");

	zassert_equal(p->m_a, 123456789, "");
	zassert_equal(p->m_b, 987654321, "");
}

} // namespace

void test_main()
{
	ztest_test_suite(zpp_mem_slab_tests,
			 ztest_unit_test(test_mem_slab)
		);

	ztest_run_test_suite(zpp_mem_slab_tests);
}
