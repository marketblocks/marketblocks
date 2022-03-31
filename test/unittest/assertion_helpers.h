#pragma once

#include <gtest/gtest.h>

#include "common/types/result.h"
#include "trading/order_book.h"
#include "trading/pair_stats.h"
#include "trading/order_description.h"

namespace mb::test
{
	template<typename T>
	void no_assert(const T&, const T&)
	{
	}

	template<typename T>
	void default_expect_eq(const T& lhs, const T& rhs)
	{
		EXPECT_EQ(lhs, rhs);
	}

	template<typename T, typename ValueAsserter>
	void assert_result_equal(const result<T>& lhs, const result<T>& rhs, ValueAsserter valueAsserter)
	{
		ASSERT_EQ(lhs.is_success(), rhs.is_success());

		if (lhs.is_success())
		{
			valueAsserter(lhs.value(), rhs.value());
		}
		else
		{
			ASSERT_EQ(lhs.error(), rhs.error());
		}
	}

	void assert_order_book_entry_eq(const order_book_entry& lhs, const order_book_entry& rhs);
	void assert_order_book_state_eq(const order_book_state& lhs, const order_book_state& rhs);
	void assert_pair_stats_eq(const pair_stats& lhs, const pair_stats& rhs);
	void assert_order_description_eq(const std::vector<order_description>& lhs, const std::vector<order_description>& rhs);
}