#pragma once

#include <gtest/gtest.h>

#include "common/types/result.h"
#include "common/types/partial_data_result.h"
#include "trading/order_book.h"
#include "trading/ohlcv_data.h"
#include "trading/order_description.h"
#include "trading/trade_update.h"

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

	template<typename T, typename ValueAsserter>
	auto create_vector_equal_asserter(ValueAsserter valueAsserter)
	{
		return [valueAsserter](const std::vector<T>& lhs, const std::vector<T>& rhs)
		{
			ASSERT_EQ(lhs.size(), rhs.size());

			for (int i = 0; i < lhs.size(); ++i)
			{
				valueAsserter(lhs[i], rhs[i]);
			}
		};
	}

	template<typename DataType, typename SequenceId, typename ValueAsserter>
	auto create_partial_data_result_asserter(ValueAsserter valueAsserter)
	{
		return [valueAsserter](const partial_data_result<DataType, SequenceId>& lhs, const partial_data_result<DataType, SequenceId>& rhs)
		{
			ASSERT_EQ(lhs.id(), rhs.id());
			create_vector_equal_asserter<DataType>(valueAsserter)(lhs.data(), rhs.data());
		};
	}

	void assert_order_book_entry_eq(const order_book_entry& lhs, const order_book_entry& rhs);
	void assert_order_book_state_eq(const order_book_state& lhs, const order_book_state& rhs);
	void assert_ohlcv_data_eq(const ohlcv_data& lhs, const ohlcv_data& rhs);
	void assert_order_description_eq(const order_description& lhs, const order_description& rhs);
	void assert_trade_update_eq(const trade_update& lhs, const trade_update& rhs);
}