#include <gtest/gtest.h>

#include "unittest/assertion_helpers.h"
#include "exchanges/websockets/order_book_cache.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	void assert_snapshot_equal_to_maps(const ask_map& asks, const bid_map& bids, const order_book_state& snapshot, int expectedAsksDepth, int expectedBidsDepth)
	{
		ASSERT_EQ(snapshot.asks().size(), expectedAsksDepth);
		ASSERT_EQ(snapshot.bids().size(), expectedBidsDepth);

		auto asksIt = asks.begin();
		auto bidsIt = bids.begin();

		for (int i = 0; i < snapshot.depth(); ++i)
		{
			if (i < snapshot.asks().size() && asksIt != asks.end())
			{
				assert_order_book_entry_eq(snapshot.asks()[i], asksIt->second);
				++asksIt;
			}

			if (i < snapshot.bids().size() && bidsIt != bids.end())
			{
				assert_order_book_entry_eq(snapshot.bids()[i], bidsIt->second);
				++bidsIt;
			}
		}
	}
}

namespace mb::test
{
	TEST(OrderBookCache, ValidInputs)
	{
		ask_map asks
		{
			{ "30964.51", order_book_entry{ 30964.51, 0.105 } },
			{ "30986.75", order_book_entry{ 30986.75, 0.03 } },
			{ "30995.72", order_book_entry{ 30995.72, 0.160 } },
		};

		bid_map bids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
			{ "30926.01", order_book_entry{ 30926.01, 0.171 } }
		};

		order_book_cache cache{ asks, bids, 3 };

		assert_snapshot_equal_to_maps(asks, bids, cache.snapshot(), 3, 3);
	}

	TEST(OrderBookCache, EntriesLessThanDepth)
	{
		ask_map asks
		{
			{ "30964.51", order_book_entry{ 30964.51, 0.105 } },
			{ "30986.75", order_book_entry{ 30986.75, 0.03 } },
			{ "30995.72", order_book_entry{ 30995.72, 0.160 } },
		};

		bid_map bids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
			{ "30926.01", order_book_entry{ 30926.01, 0.171 } }
		};

		order_book_cache cache{ asks, bids, 5 };

		assert_snapshot_equal_to_maps(asks, bids, cache.snapshot(), 3, 3);
	}

	TEST(OrderBookCache, DifferentNumberOfAsksAndBids)
	{
		ask_map asks
		{
			{ "30964.51", order_book_entry{ 30964.51, 0.105 } },
			{ "30986.75", order_book_entry{ 30986.75, 0.03 } },
			{ "30995.72", order_book_entry{ 30995.72, 0.160 } },
		};

		bid_map bids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
		};

		order_book_cache cache{ asks, bids, 3 };

		assert_snapshot_equal_to_maps(asks, bids, cache.snapshot(), 3, 2);
	}

	TEST(OrderBookCache, MoreEntriesThanSpecificedDepthGetsTrimmed)
	{
		ask_map asks
		{
			{ "30964.51", order_book_entry{ 30964.51, 0.105 } },
			{ "30986.75", order_book_entry{ 30986.75, 0.03 } },
			{ "30995.72", order_book_entry{ 30995.72, 0.160 } },
		};

		bid_map bids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
			{ "30926.01", order_book_entry{ 30926.01, 0.171 } }
		};

		order_book_cache cache{ asks, bids, 2 };

		assert_snapshot_equal_to_maps(asks, bids, cache.snapshot(3), 2, 2);
	}

	TEST(OrderBookCache, SnapshotDepthSpecifiesNumberOfEntries)
	{
		ask_map asks
		{
			{ "30964.51", order_book_entry{ 30964.51, 0.105 } },
			{ "30986.75", order_book_entry{ 30986.75, 0.03 } },
			{ "30995.72", order_book_entry{ 30995.72, 0.160 } },
		};

		bid_map bids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
			{ "30926.01", order_book_entry{ 30926.01, 0.171 } }
		};

		order_book_cache cache{ asks, bids, 3 };

		assert_snapshot_equal_to_maps(asks, bids, cache.snapshot(1), 1, 1);
	}

	TEST(OrderBookCache, CachingNewEntryInsertsInCorrectPositionAndMaintainsDepth)
	{
		ask_map asks
		{
			{ "30964.51", order_book_entry{ 30964.51, 0.105 } },
			{ "30986.75", order_book_entry{ 30986.75, 0.03 } },
			{ "30995.72", order_book_entry{ 30995.72, 0.160 } },
		};

		bid_map bids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
			{ "30926.01", order_book_entry{ 30926.01, 0.171 } }
		};

		order_book_cache cache{ asks, bids, 3 };

		order_book_cache_entry newEntry
		{
			order_book_side::BID,
			"30948.32",
			order_book_entry{ 30948.32, 0.025 }
		};

		bid_map expectedBids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ newEntry.price, newEntry.entry },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } }
		};

		cache.update_cache(newEntry);

		assert_snapshot_equal_to_maps(asks, expectedBids, cache.snapshot(), 3, 3);
	}

	TEST(OrderBookCache, CachingEntryAtExistingPriceUpdatesVolume)
	{
		ask_map asks
		{
			{ "30964.51", order_book_entry{ 30964.51, 0.105 } },
			{ "30986.75", order_book_entry{ 30986.75, 0.03 } },
		};

		bid_map bids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
		};

		order_book_cache cache{ asks, bids, 2 };

		order_book_cache_entry newEntry
		{
			order_book_side::BID,
			"30944.65",
			order_book_entry{ 30944.65, 0.025 }
		};

		bid_map expectedBids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.025 } }
		};

		cache.update_cache(newEntry);

		assert_snapshot_equal_to_maps(asks, expectedBids, cache.snapshot(), 2, 2);
	}

	TEST(OrderBookCache, RemovesCorrectlyRemovesEntry)
	{
		ask_map asks
		{
			{ "30964.51", order_book_entry{ 30964.51, 0.105 } },
			{ "30986.75", order_book_entry{ 30986.75, 0.03 } },
			{ "30995.72", order_book_entry{ 30995.72, 0.160 } },
		};

		bid_map bids
		{
			{ "30956.20", order_book_entry{ 30956.20, 0.105 } },
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
			{ "30926.01", order_book_entry{ 30926.01, 0.171 } }
		};

		order_book_cache cache{ asks, bids, 3 };

		std::string priceToRemove{ "30956.20" };
		order_book_side side = order_book_side::BID;

		bid_map expectedBids
		{
			{ "30944.65", order_book_entry{ 30944.65, 0.03 } },
			{ "30926.01", order_book_entry{ 30926.01, 0.171 } }
		};

		cache.remove(priceToRemove, side);

		assert_snapshot_equal_to_maps(asks, expectedBids, cache.snapshot(), 3, 2);
	}
}
