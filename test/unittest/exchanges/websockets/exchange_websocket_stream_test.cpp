#include <gtest/gtest.h>

#include "exchanges/websockets/exchange_websocket_stream.h"
#include "unittest/mocks.h"
#include "unittest/assertion_helpers.h"

namespace
{
	using namespace mb;
	using namespace mb::test;
	
	mock_exchange_websocket_stream create_mock_stream()
	{
		std::unique_ptr<mock_websocket_connection_factory> mockConnectionFactory{ std::make_unique<mock_websocket_connection_factory>() };
		return mock_exchange_websocket_stream{ "test", "test", std::move(mockConnectionFactory) };
	}
}

namespace mb::test
{
	using ::testing::_;
	using ::testing::Return;

	TEST(ExchangeWebsocketStream, UpdatePriceSetsPrice)
	{
		std::string subId{ "testSubId" };
		double price{ 1.5 };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_update_price(subId, price);

		EXPECT_EQ(price, test.get_price(tradable_pair{ "test", "test" }));
	}

	TEST(ExchangeWebsocketStream, UpdateOhlcvSetsOhlcv)
	{
		std::string subId{ "testSubId" };
		ohlcv_data ohlcvData{ 1, 2, 3, 4, 5, 6 };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_update_ohlcv(subId, ohlcvData);

		assert_ohlcv_data_eq(ohlcvData, test.get_last_candle(tradable_pair{ "test", "test" }, ohlcv_interval::D1));
	}

	TEST(ExchangeWebsocketStream, InitialiseOrderBookCreatesOrderBook)
	{
		std::string subId{ "testSubId" };

		ask_cache asks
		{
			order_book_entry{1.0, 2.0, order_book_side::ASK},
			order_book_entry{1.1, 3.0, order_book_side::ASK}
		};
		bid_cache bids
		{
			order_book_entry{0.9, 4.0, order_book_side::BID},
			order_book_entry{0.8, 5.0, order_book_side::BID}
		};

		order_book_state expectedState
		{
			{ asks.begin(), asks.end() },
			{ bids.begin(), bids.end() }
		};

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_initialise_order_book(subId, order_book_cache{asks, bids});

		assert_order_book_state_eq(expectedState, test.get_order_book(tradable_pair{"test", "test"}));
	}

	TEST(ExchangeWebsocketStream, UpdateOrderBookUpdatesCache)
	{
		std::string subId{ "testSubId" };

		ask_cache asks
		{
			order_book_entry{1.0, 2.0, order_book_side::ASK},
			order_book_entry{1.1, 3.0, order_book_side::ASK}
		};
		bid_cache bids
		{
			order_book_entry{0.9, 4.0, order_book_side::BID},
			order_book_entry{0.8, 5.0, order_book_side::BID}
		};

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_initialise_order_book(subId, order_book_cache{ asks, bids });

		order_book_entry newEntry{ 1.0, 0.0, order_book_side::ASK };
		test.expose_update_order_book(subId, newEntry);

		order_book_state expectedState
		{
			{ order_book_entry{1.1, 3.0, order_book_side::ASK} },
			{ bids.begin(), bids.end() }
		};

		assert_order_book_state_eq(expectedState, test.get_order_book(tradable_pair{ "test", "test" }));
	}

	TEST(ExchangeWebsocketStream, CallingUpdateOrderBookBeforeInitialiseCreatesEmptyBook)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		order_book_entry newEntry{ 1.0, 2.0, order_book_side::ASK };
		
		order_book_state expectedState
		{
			{ newEntry },
			{}
		};

		test.expose_update_order_book(subId, newEntry);

		assert_order_book_state_eq(expectedState, test.get_order_book(tradable_pair{ "test", "test" }));
	}

	TEST(ExchangeWebsocketStream, SubscriptionStatusIsInitiallyUnsubscribed)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		EXPECT_EQ(
			subscription_status::UNSUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_price_sub(tradable_pair{ "test", "test" })));
	}

	TEST(ExchangeWebsocketStream, UpdatePriceSetsSubscribedStatus)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_update_price(subId, 1.0);

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_price_sub(tradable_pair{ "test", "test" })));
	}

	TEST(ExchangeWebsocketStream, UpdateOhlcvSetsSubscribedStatus)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_update_ohlcv(subId, ohlcv_data{1, 2, 3, 4, 5, 6});

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_ohlcv_sub(tradable_pair{ "test", "test" }, ohlcv_interval::D1)));
	}

	TEST(ExchangeWebsocketStream, InitialiseOrderBookSetsSubscribedStatus)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_initialise_order_book(subId, order_book_cache{ {},{} });

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_order_book_sub(tradable_pair{ "test", "test" })));
	}

	TEST(ExchangeWebsocketStream, SetUnsubscribedClearsCachedPriceValue)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_update_price(subId, 1.0);
		test.expose_set_unsubscribed(subId, websocket_channel::PRICE);

		EXPECT_EQ(0.0, test.get_price(tradable_pair{ "test", "test" }));
	}

	TEST(ExchangeWebsocketStream, SetUnsubscribedClearsCachedOhlcvValue)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_update_ohlcv(subId, ohlcv_data{ 1, 2, 3, 4, 5, 6 });
		test.expose_set_unsubscribed(subId, websocket_channel::OHLCV);

		assert_ohlcv_data_eq(ohlcv_data{}, test.get_last_candle(tradable_pair{"test", "test"}, ohlcv_interval::D1));
	}

	TEST(ExchangeWebsocketStream, SetUnsubscribedClearsCachedOrderBook)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		ask_cache asks
		{
			order_book_entry{1.0, 2.0, order_book_side::ASK},
			order_book_entry{1.1, 3.0, order_book_side::ASK}
		};
		bid_cache bids
		{
			order_book_entry{0.9, 4.0, order_book_side::BID},
			order_book_entry{0.8, 5.0, order_book_side::BID}
		};

		test.expose_initialise_order_book(subId, order_book_cache{ asks, bids });
		test.expose_set_unsubscribed(subId, websocket_channel::ORDER_BOOK);

		assert_order_book_state_eq(order_book_state{ {},{} }, test.get_order_book(tradable_pair{ "test", "test" }));
	}

	TEST(ExchangeWebsocketStream, SetUnsubscribedSetsUnsubscribedStatus)
	{
		std::string subId{ "testSubId" };

		mock_exchange_websocket_stream test{ create_mock_stream() };
		EXPECT_CALL(test, generate_subscription_id(_)).WillRepeatedly(Return(subId));

		test.expose_update_price(subId, 1.0);
		test.expose_set_unsubscribed(subId, websocket_channel::PRICE);

		EXPECT_EQ(
			subscription_status::UNSUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_price_sub(tradable_pair{ "test", "test" })));
	}
}