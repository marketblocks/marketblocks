#include "assertion_helpers.h"

namespace mb::test
{
	void assert_order_book_entry_eq(const order_book_entry& lhs, const order_book_entry& rhs)
	{
		EXPECT_DOUBLE_EQ(lhs.price(), rhs.price());
		EXPECT_DOUBLE_EQ(lhs.volume(), rhs.volume());
	}

	void assert_order_book_state_eq(const order_book_state& lhs, const order_book_state& rhs)
	{
		ASSERT_EQ(lhs.asks().size(), rhs.asks().size());
		ASSERT_EQ(lhs.bids().size(), rhs.bids().size());

		for (int i = 0; i < lhs.asks().size(); ++i)
		{
			assert_order_book_entry_eq(lhs.asks()[i], rhs.asks()[i]);
		}

		for (int i = 0; i < lhs.bids().size(); ++i)
		{
			assert_order_book_entry_eq(lhs.bids()[i], rhs.bids()[i]);
		}
	}

	void assert_ohlcv_data_eq(const ohlcv_data& lhs, const ohlcv_data& rhs)
	{
		EXPECT_EQ(lhs.time_stamp(), rhs.time_stamp());
		EXPECT_DOUBLE_EQ(lhs.open(), rhs.open());
		EXPECT_DOUBLE_EQ(lhs.high(), rhs.high());
		EXPECT_DOUBLE_EQ(lhs.low(), rhs.low());
		EXPECT_DOUBLE_EQ(lhs.close(), rhs.close());
		EXPECT_DOUBLE_EQ(lhs.volume(), rhs.volume());
	}

	void assert_order_description_eq(const order_description& lhs, const order_description& rhs)
	{
		EXPECT_EQ(lhs.time_stamp(), rhs.time_stamp());
		EXPECT_EQ(lhs.order_id(), rhs.order_id());
		EXPECT_EQ(lhs.pair_name(), rhs.pair_name());
		EXPECT_EQ(lhs.action(), rhs.action());
		EXPECT_DOUBLE_EQ(lhs.price(), rhs.price());
		EXPECT_DOUBLE_EQ(lhs.volume(), rhs.volume());
	}

	void assert_trade_update_eq(const trade_update& lhs, const trade_update& rhs)
	{
		EXPECT_EQ(lhs.time_stamp(), rhs.time_stamp());
		EXPECT_DOUBLE_EQ(lhs.price(), rhs.price());
		EXPECT_DOUBLE_EQ(lhs.volume(), rhs.volume());
	}
}