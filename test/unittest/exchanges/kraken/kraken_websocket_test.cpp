#include <gtest/gtest.h>

#include "exchanges/kraken/kraken_websocket.h"
#include "unittest/assertion_helpers.h"
#include "test_data/test_data_constants.h"
#include "common/file/file.h"

namespace mb::test
{
	using namespace mb::internal;

	TEST(KrakenWebsocket, CreatesCorrectOrderBookSubscriptionMessage)
	{
		kraken_websocket_stream krakenWebsocket{};
		std::vector<tradable_pair> tradablePairs
		{
			tradable_pair{ "BTC", "GBP" },
			tradable_pair{ "ETH", "USD" }
		};
		std::string expectedMessage{ "{\"event\":\"subscribe\",\"pair\":[\"BTC/GBP\",\"ETH/USD\"],\"subscription\":{\"name\":\"book\"}}" };

		std::string actualMessage{ krakenWebsocket.get_order_book_subscription_message(tradablePairs) };
		ASSERT_EQ(expectedMessage, actualMessage);
	}

	TEST(KrakenWebsocket, CreatesCorrectOrderBookUnsubscriptionMessage)
	{
		kraken_websocket_stream krakenWebsocket{};
		std::vector<tradable_pair> tradablePairs
		{
			tradable_pair{ "BTC", "GBP" },
			tradable_pair{ "ETH", "USD" }
		};
		std::string expectedMessage{ "{\"event\":\"unsubscribe\",\"pair\":[\"BTC/GBP\",\"ETH/USD\"],\"subscription\":{\"name\":\"book\"}}" };

		std::string actualMessage{ krakenWebsocket.get_order_book_unsubscription_message(tradablePairs) };
		ASSERT_EQ(expectedMessage, actualMessage);
	}

	TEST(KrakenWebsocket, SnapshotMessageInitializesOrderBook)
	{
		kraken_websocket_stream krakenWebsocket{};

		std::string snapshotMessage{ read_file(kraken_websocket_test_data_path("order_book_snapshot.json")) };

		order_book_state expectedOrderBook
		{
			{
				order_book_entry{ 5541.30, 2.5070 },
				order_book_entry{ 5541.80, 0.330 }
			},
			{
				order_book_entry{ 5541.20, 1.5290 },
				order_book_entry{ 5539.90, 0.300 }
			}
		};

		krakenWebsocket.on_message(snapshotMessage);

		order_book_state actualOrderBook{ krakenWebsocket.get_order_book(tradable_pair{ "XBT", "USD" }) };

		assert_order_book_state_eq(expectedOrderBook, actualOrderBook);
	}

	TEST(KrakenWebsocket, SnapshotMessageAddsPairToMessageQueue)
	{
		kraken_websocket_stream krakenWebsocket{};
		tradable_pair expectedPair{ "XBT", "USD" };

		std::string snapshotMessage{ read_file(kraken_websocket_test_data_path("order_book_snapshot.json")) };
		krakenWebsocket.on_message(snapshotMessage);
		auto& messageQueue = krakenWebsocket.get_order_book_message_queue();

		ASSERT_EQ(messageQueue.pop(), expectedPair);
	}

	TEST(KrakenWebsocket, UpdateMessageCorrectlyUpdatesOrderBook1)
	{
		kraken_websocket_stream krakenWebsocket{};
		tradable_pair expectedPair{ "XBT", "USD" };

		std::string snapshotMessage{ read_file(kraken_websocket_test_data_path("order_book_snapshot.json")) };
		krakenWebsocket.on_message(snapshotMessage);

		std::string updateMessage{ read_file(kraken_websocket_test_data_path("order_book_update_1.json")) };
		krakenWebsocket.on_message(updateMessage);

		order_book_state expectedOrderBook
		{
			{
				order_book_entry{ 5541.30, 1.6008 },
				order_book_entry{ 5541.50, 0.401 }
			},
			{
				order_book_entry{ 5541.20, 1.5290 },
				order_book_entry{ 5539.90, 0.300 }
			}
		};

		order_book_state actualOrderBook{ krakenWebsocket.get_order_book(expectedPair) };

		assert_order_book_state_eq(expectedOrderBook, actualOrderBook);
	}

	TEST(KrakenWebsocket, UpdateMessageCorrectlyUpdatesOrderBook2)
	{
		kraken_websocket_stream krakenWebsocket{};
		tradable_pair expectedPair{ "XBT", "USD" };

		std::string snapshotMessage{ read_file(kraken_websocket_test_data_path("order_book_snapshot.json")) };
		krakenWebsocket.on_message(snapshotMessage);

		std::string updateMessage{ read_file(kraken_websocket_test_data_path("order_book_update_2.json")) };
		krakenWebsocket.on_message(updateMessage);

		order_book_state expectedOrderBook
		{
			{
				order_book_entry{ 5541.30, 1.6008 },
				order_book_entry{ 5541.80, 0.330 }
			},
			{
				order_book_entry{ 5541.20, 1.5290 },
				order_book_entry{ 5539.90, 0.1025 }
			}
		};

		order_book_state actualOrderBook{ krakenWebsocket.get_order_book(expectedPair) };

		assert_order_book_state_eq(expectedOrderBook, actualOrderBook);
	}

	TEST(KrakenWebsocket, ReplaceMessageCorrectlyUpdatesOrderBook)
	{
		kraken_websocket_stream krakenWebsocket{};
		tradable_pair expectedPair{ "XBT", "USD" };

		std::string snapshotMessage{ read_file(kraken_websocket_test_data_path("order_book_snapshot.json")) };
		krakenWebsocket.on_message(snapshotMessage);

		std::string updateMessage{ read_file(kraken_websocket_test_data_path("order_book_replace.json")) };
		krakenWebsocket.on_message(updateMessage);

		order_book_state expectedOrderBook
		{
			{
				order_book_entry{ 5541.30, 2.5070 },
				order_book_entry{ 5542.30, 1.034450 }
			},
			{
				order_book_entry{ 5541.20, 1.5290 },
				order_book_entry{ 5539.90, 0.300 }
			}
		};

		order_book_state actualOrderBook{ krakenWebsocket.get_order_book(expectedPair) };

		assert_order_book_state_eq(expectedOrderBook, actualOrderBook);
	}
}