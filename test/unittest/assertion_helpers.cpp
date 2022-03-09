#include "assertion_helpers.h"

namespace cb::test
{
	void assert_order_book_entry_eq(const order_book_entry& lhs, const order_book_entry& rhs)
	{
		EXPECT_EQ(lhs.side(), rhs.side());
		EXPECT_DOUBLE_EQ(lhs.price(), rhs.price());
		EXPECT_DOUBLE_EQ(lhs.volume(), rhs.volume());
	}

	void assert_order_book_level_eq(const order_book_level& lhs, const order_book_level& rhs)
	{
		assert_order_book_entry_eq(lhs.ask(), rhs.ask());
		assert_order_book_entry_eq(lhs.bid(), rhs.bid());
	}

	void assert_order_book_state_eq(const order_book_state& lhs, const order_book_state& rhs)
	{
		ASSERT_EQ(lhs.depth(), rhs.depth());

		for (int i = 0; i < lhs.depth(); ++i)
		{
			assert_order_book_level_eq(lhs.level(i), rhs.level(i));
		}
	}

	void assert_pair_stats_eq(const pair_stats& lhs, const pair_stats& rhs)
	{
		EXPECT_DOUBLE_EQ(lhs.high(), rhs.high());
		EXPECT_DOUBLE_EQ(lhs.low(), rhs.low());
		EXPECT_DOUBLE_EQ(lhs.open(), rhs.open());
		EXPECT_DOUBLE_EQ(lhs.volume(), rhs.volume());
	}

	void assert_order_description_eq(const std::vector<cb::order_description>& lhs, const std::vector<cb::order_description>& rhs)
	{
		ASSERT_EQ(lhs.size(), rhs.size());

		for (int i = 0; i < lhs.size(); ++i)
		{
			EXPECT_EQ(lhs[i].order_id(), rhs[i].order_id());
			EXPECT_EQ(lhs[i].pair_name(), rhs[i].pair_name());
			EXPECT_EQ(lhs[i].action(), rhs[i].action());
			EXPECT_DOUBLE_EQ(lhs[i].price(), rhs[i].price());
			EXPECT_DOUBLE_EQ(lhs[i].volume(), rhs[i].volume());
		}
	}
}