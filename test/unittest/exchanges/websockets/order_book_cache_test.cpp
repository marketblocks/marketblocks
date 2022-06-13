#include <gtest/gtest.h>

#include "unittest/assertion_helpers.h"
#include "exchanges/websockets/order_book_cache.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	void assert_snapshot_equal_to_maps(const ask_cache& asks, const bid_cache& bids, const order_book_state& snapshot)
	{
		ASSERT_EQ(asks.size(), snapshot.asks().size());
		ASSERT_EQ(bids.size(), snapshot.bids().size());

		auto asksIt = asks.begin();
		auto bidsIt = bids.begin();

		for (int i = 0; i < snapshot.depth(); ++i)
		{
			if (i < snapshot.asks().size() && asksIt != asks.end())
			{
				assert_order_book_entry_eq(snapshot.asks()[i], *asksIt);
				++asksIt;
			}

			if (i < snapshot.bids().size() && bidsIt != bids.end())
			{
				assert_order_book_entry_eq(snapshot.bids()[i], *bidsIt);
				++bidsIt;
			}
		}
	}
}

namespace mb::test
{
	TEST(OrderBookCache, ValidInputs)
	{
		ask_cache asks
		{
			order_book_entry{ 30964.51, 0.105, order_book_side::ASK },
			order_book_entry{ 30986.75, 0.03, order_book_side::ASK },
			order_book_entry{ 30995.72, 0.160, order_book_side::ASK },
		};

		bid_cache bids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID },
			order_book_entry{ 30944.65, 0.03, order_book_side::BID },
			order_book_entry{ 30926.01, 0.171, order_book_side::BID }
		};

		order_book_cache cache{ asks, bids };

		assert_snapshot_equal_to_maps(asks, bids, cache.snapshot());
	}

	TEST(OrderBookCache, DifferentNumberOfAsksAndBids)
	{
		ask_cache asks
		{
			order_book_entry{ 30964.51, 0.105, order_book_side::ASK },
			order_book_entry{ 30986.75, 0.03, order_book_side::ASK },
			order_book_entry{ 30995.72, 0.160, order_book_side::ASK },
		};

		bid_cache bids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID },
			order_book_entry{ 30944.65, 0.03, order_book_side::BID },
		};

		order_book_cache cache{ asks, bids };

		assert_snapshot_equal_to_maps(asks, bids, cache.snapshot());
	}

	TEST(OrderBookCache, SnapshotDepthSpecifiesNumberOfEntries)
	{
		ask_cache asks
		{
			order_book_entry{ 30964.51, 0.105, order_book_side::ASK },
			order_book_entry{ 30986.75, 0.03, order_book_side::ASK },
			order_book_entry{ 30995.72, 0.160, order_book_side::ASK },
		};

		bid_cache bids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID },
			order_book_entry{ 30944.65, 0.03, order_book_side::BID },
			order_book_entry{ 30926.01, 0.171, order_book_side::BID }
		};

		order_book_cache cache{ asks, bids };

		ask_cache expectedAsks
		{
			order_book_entry{ 30964.51, 0.105, order_book_side::ASK }
		};

		bid_cache expectedBids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID }
		};

		assert_snapshot_equal_to_maps(expectedAsks, expectedBids, cache.snapshot(1));
	}

	TEST(OrderBookCache, CachingNewEntryInsertsInCorrectPosition)
	{
		ask_cache asks
		{
			order_book_entry{ 30964.51, 0.105, order_book_side::ASK },
			order_book_entry{ 30986.75, 0.03, order_book_side::ASK },
			order_book_entry{ 30995.72, 0.160, order_book_side::ASK },
		};

		bid_cache bids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID },
			order_book_entry{ 30944.65, 0.03, order_book_side::BID },
			order_book_entry{ 30926.01, 0.171, order_book_side::BID }
		};

		order_book_cache cache{ asks, bids };

		order_book_entry newEntry{ 30948.32, 0.025, order_book_side::BID };

		bid_cache expectedBids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID },
			newEntry,
			order_book_entry{ 30944.65, 0.03, order_book_side::BID },
			order_book_entry{ 30926.01, 0.171, order_book_side::BID }
		};

		cache.update_cache(newEntry);

		assert_snapshot_equal_to_maps(asks, expectedBids, cache.snapshot());
	}

	TEST(OrderBookCache, CachingEntryAtExistingPriceUpdatesVolume)
	{
		ask_cache asks
		{
			order_book_entry{ 30964.51, 0.105, order_book_side::ASK },
			order_book_entry{ 30986.75, 0.03, order_book_side::ASK },
			order_book_entry{ 30995.72, 0.160, order_book_side::ASK },
		};

		bid_cache bids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID },
			order_book_entry{ 30944.65, 0.03, order_book_side::BID },
			order_book_entry{ 30926.01, 0.171, order_book_side::BID }
		};

		order_book_cache cache{ asks, bids };

		order_book_entry newEntry{ 30944.65, 0.025, order_book_side::BID };

		bid_cache expectedBids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID },
			order_book_entry{ 30944.65, 0.025, order_book_side::BID },
			order_book_entry{ 30926.01, 0.171, order_book_side::BID }
		};

		cache.update_cache(newEntry);

		assert_snapshot_equal_to_maps(asks, expectedBids, cache.snapshot());
	}

	TEST(OrderBookCache, CachingEntryAtExistingPriceWithZeroVolumeRemoves)
	{
		ask_cache asks
		{
			order_book_entry{ 30964.51, 0.105, order_book_side::ASK },
			order_book_entry{ 30986.75, 0.03, order_book_side::ASK },
			order_book_entry{ 30995.72, 0.160, order_book_side::ASK },
		};

		bid_cache bids
		{
			order_book_entry{ 30956.20, 0.105, order_book_side::BID },
			order_book_entry{ 30944.65, 0.03, order_book_side::BID },
			order_book_entry{ 30926.01, 0.171, order_book_side::BID }
		};

		order_book_cache cache{ asks, bids };

		order_book_entry newEntry{ 30995.72, 0.0, order_book_side::ASK };

		ask_cache expectedAsks
		{
			order_book_entry{ 30964.51, 0.105, order_book_side::ASK },
			order_book_entry{ 30986.75, 0.03, order_book_side::ASK }
		};

		cache.update_cache(newEntry);

		assert_snapshot_equal_to_maps(expectedAsks, bids, cache.snapshot());
	}
}
