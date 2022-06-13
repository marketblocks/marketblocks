//#include <gtest/gtest.h>
//
//#include "unittest/assertion_helpers.h"
//#include "exchanges/coinbase/coinbase_websocket.h"
//#include "common/json/json.h"
//#include "common/utils/containerutils.h"
//
//namespace mb::test
//{
//	using namespace mb::internal;
//
//	TEST(CoinbaseWebsocket, CreatesCorrectOrderBookSubscriptionMessage)
//	{
//		coinbase_websocket_stream coinbaseWebsocket{};
//		std::vector<tradable_pair> tradablePairs
//		{
//			tradable_pair{ "BTC", "GBP" },
//			tradable_pair{ "ETH", "USD" }
//		};
//		std::string expectedMessage{ "{\"channels\":[\"level2\"],\"product_ids\":[\"BTC-GBP\",\"ETH-USD\"],\"type\":\"subscribe\"}" };
//
//		std::string actualMessage{ coinbaseWebsocket.get_order_book_subscription_message(tradablePairs) };
//		ASSERT_EQ(expectedMessage, actualMessage);
//	}
//
//	TEST(CoinbaseWebsocket, CreatesCorrectOrderBookUnsubscriptionMessage)
//	{
//		coinbase_websocket_stream coinbaseWebsocket{};
//		std::vector<tradable_pair> tradablePairs
//		{
//			tradable_pair{ "BTC", "GBP" },
//			tradable_pair{ "ETH", "USD" }
//		};
//		std::string expectedMessage{ "{\"channels\":[\"level2\"],\"product_ids\":[\"BTC-GBP\",\"ETH-USD\"],\"type\":\"unsubscribe\"}" };
//
//		std::string actualMessage{ coinbaseWebsocket.get_order_book_unsubscription_message(tradablePairs) };
//		ASSERT_EQ(expectedMessage, actualMessage);
//	}
//
//	TEST(CoinbaseWebsocket, SnapshotMessageInitializesOrderBook)
//	{
//		coinbase_websocket_stream coinbaseWebsocket{};
//
//		std::vector<std::vector<std::string>> bids
//		{
//			{"10101.10", "0.4505"},
//			{"10100.56", "0.2750"}
//		};
//
//		std::vector<std::vector<std::string>> asks
//		{
//			{"10102.55", "0.5775"},
//			{"10103.78", "0.5858"}
//		};
//
//		std::string snapshotMessage = json_writer{}
//			.add("type", "snapshot")
//			.add("product_id", "BTC-USD")
//			.add("bids", std::move(bids))
//			.add("asks", std::move(asks))
//			.to_string();
//
//		order_book_state expectedOrderBook
//		{
//			{
//				order_book_entry{ 10102.55, 0.5775 },
//				order_book_entry{ 10103.78, 0.5858 }
//			},
//			{
//				order_book_entry{ 10101.10, 0.4505 },
//				order_book_entry{ 10100.56, 0.2750 }
//			}
//		};
//
//		coinbaseWebsocket.on_message(snapshotMessage);
//
//		order_book_state actualOrderBook{ coinbaseWebsocket.get_order_book(tradable_pair{ "BTC", "USD" }) };
//
//		assert_order_book_state_eq(expectedOrderBook, actualOrderBook);
//	}
//
//	TEST(CoinbaseWebsocket, UpdateMessageCorrectlyUpdatesOrderBook)
//	{
//		coinbase_websocket_stream coinbaseWebsocket{};
//
//		std::vector<std::vector<std::string>> bids
//		{
//			{"10101.10", "0.4505"},
//			{"10100.56", "0.2750"}
//		};
//
//		std::vector<std::vector<std::string>> asks
//		{
//			{"10102.55", "0.5775"},
//			{"10103.78", "0.5858"}
//		};
//
//		std::string snapshotMessage = json_writer{}
//			.add("type", "snapshot")
//			.add("product_id", "BTC-USD")
//			.add("bids", std::move(bids))
//			.add("asks", std::move(asks))
//			.to_string();
//
//		coinbaseWebsocket.on_message(snapshotMessage);
//
//		std::vector<std::vector<std::string>> changes
//		{
//			{"buy", "10101.10", "0.2003"},
//			{"sell", "10103.78", "0.5220"}
//		};
//
//		std::string updateMessage = json_writer{}
//			.add("type", "l2update")
//			.add("product_id", "BTC-USD")
//			.add("time", "2019-08-14T20:42:27.265Z")
//			.add("changes", std::move(changes))
//			.to_string();
//
//		order_book_state expectedOrderBook
//		{
//			{
//				order_book_entry{ 10102.55, 0.5775 },
//				order_book_entry{ 10103.78, 0.5220 }
//			},
//			{
//				order_book_entry{ 10101.10, 0.2003 },
//				order_book_entry{ 10100.56, 0.2750 }
//			}
//		};
//
//		coinbaseWebsocket.on_message(updateMessage);
//
//		order_book_state actualOrderBook{ coinbaseWebsocket.get_order_book(tradable_pair{ "BTC", "USD" }) };
//
//		assert_order_book_state_eq(expectedOrderBook, actualOrderBook);
//	}
//
//	TEST(CoinbaseWebsocket, UpdateMessageWith0VolumeRemovesEntry)
//	{
//		coinbase_websocket_stream coinbaseWebsocket{};
//
//		std::vector<std::vector<std::string>> bids
//		{
//			{"10101.10", "0.4505"},
//			{"10100.56", "0.2750"}
//		};
//
//		std::vector<std::vector<std::string>> asks
//		{
//			{"10102.55", "0.5775"},
//			{"10103.78", "0.5858"}
//		};
//
//		std::string snapshotMessage = json_writer{}
//			.add("type", "snapshot")
//			.add("product_id", "BTC-USD")
//			.add("bids", std::move(bids))
//			.add("asks", std::move(asks))
//			.to_string();
//
//		coinbaseWebsocket.on_message(snapshotMessage);
//
//		std::vector<std::vector<std::string>> changes
//		{
//			{"buy", "10101.10", "0.0"}
//		};
//
//		std::string updateMessage = json_writer{}
//			.add("type", "l2update")
//			.add("product_id", "BTC-USD")
//			.add("time", "2019-08-14T20:42:27.265Z")
//			.add("changes", std::move(changes))
//			.to_string();
//
//		order_book_state expectedOrderBook
//		{
//			{
//				order_book_entry{ 10102.55, 0.5775 },
//				order_book_entry{ 10103.78, 0.5858 }
//			},
//			{
//				order_book_entry{ 10100.56, 0.2750 }
//			}
//		};
//
//		coinbaseWebsocket.on_message(updateMessage);
//
//		order_book_state actualOrderBook{ coinbaseWebsocket.get_order_book(tradable_pair{ "BTC", "USD" }) };
//
//		assert_order_book_state_eq(expectedOrderBook, actualOrderBook);
//	}
//}