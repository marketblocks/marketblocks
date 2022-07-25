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

	TEST(ExchangeWebsocketStream, UpdateTradeSetsTrade)
	{
		tradable_pair pair{ "test", "test" };
		trade_update trade{ 1, 2.0, 3.0 };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_update_trade(pair.to_string(), trade);

		assert_trade_update_eq(trade, test.get_last_trade(pair));
	}

	TEST(ExchangeWebsocketStream, UpdateOhlcvSetsOhlcv)
	{
		tradable_pair pair{ "test", "test" };
		ohlcv_interval interval{ ohlcv_interval::D1 };
		ohlcv_data ohlcvData{ 1, 2, 3, 4, 5, 6 };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_update_ohlcv(pair.to_string(), interval, ohlcvData);

		assert_ohlcv_data_eq(ohlcvData, test.get_last_candle(pair, interval));
	}

	TEST(ExchangeWebsocketStream, InitialiseOrderBookCreatesOrderBook)
	{
		tradable_pair pair{ "test", "test" };

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
			1,
			{ asks.begin(), asks.end() },
			{ bids.begin(), bids.end() }
		};

		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_initialise_order_book(pair.to_string(), order_book_cache{1, asks, bids});

		assert_order_book_state_eq(expectedState, test.get_order_book(pair));
	}

	TEST(ExchangeWebsocketStream, UpdateOrderBookUpdatesCache)
	{
		tradable_pair pair{ "test", "test" };

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

		test.expose_initialise_order_book(pair.to_string(), order_book_cache{1, asks, bids});

		order_book_entry newEntry{ 1.0, 0.0, order_book_side::ASK };
		test.expose_update_order_book(pair.to_string(), 2, newEntry);

		order_book_state expectedState
		{
			2,
			{ order_book_entry{1.1, 3.0, order_book_side::ASK} },
			{ bids.begin(), bids.end() }
		};

		assert_order_book_state_eq(expectedState, test.get_order_book(pair));
	}

	TEST(ExchangeWebsocketStream, CallingUpdateOrderBookBeforeInitialiseCreatesEmptyBook)
	{
		tradable_pair pair{ "test", "test" };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		order_book_entry newEntry{ 1.0, 2.0, order_book_side::ASK };
		
		order_book_state expectedState
		{
			1,
			{ newEntry },
			{}
		};

		test.expose_update_order_book(pair.to_string(), 1, newEntry);

		assert_order_book_state_eq(expectedState, test.get_order_book(pair));
	}

	TEST(ExchangeWebsocketStream, SubscriptionStatusIsInitiallyUnsubscribed)
	{
		tradable_pair pair{ "test", "test" };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		EXPECT_EQ(
			subscription_status::UNSUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_trade_sub(pair)));
	}

	TEST(ExchangeWebsocketStream, UpdateTradeSetsSubscribedStatus)
	{
		tradable_pair pair{ "test", "test" };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_update_trade(pair.to_string(), trade_update{1, 2.0, 3.0});

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_trade_sub(pair)));
	}

	TEST(ExchangeWebsocketStream, UpdateOhlcvSetsSubscribedStatus)
	{
		tradable_pair pair{ "test", "test" };
		ohlcv_interval interval{ ohlcv_interval::D1 };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_update_ohlcv(pair.to_string(), interval, ohlcv_data{1, 2, 3, 4, 5, 6});

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_ohlcv_sub(pair, interval)));
	}

	TEST(ExchangeWebsocketStream, InitialiseOrderBookSetsSubscribedStatus)
	{
		tradable_pair pair{ "test", "test" };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_initialise_order_book(pair.to_string(), order_book_cache{0,{},{}});

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_order_book_sub(pair)));
	}

	TEST(ExchangeWebsocketStream, SetUnsubscribedClearsCachedTrade)
	{
		tradable_pair pair{ "test", "test" };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_update_trade(pair.to_string(), trade_update{ 1, 2.0, 3.0 });
		test.expose_set_unsubscribed(named_subscription::create_trade_sub(pair.to_string()));

		assert_trade_update_eq(trade_update{0,0,0}, test.get_last_trade(pair));
	}

	TEST(ExchangeWebsocketStream, SetUnsubscribedClearsCachedOhlcvValue)
	{
		tradable_pair pair{ "test", "test" };
		ohlcv_interval interval{ ohlcv_interval::D1 };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_update_ohlcv(pair.to_string(), interval, ohlcv_data{1, 2, 3, 4, 5, 6});
		test.expose_set_unsubscribed(named_subscription::create_ohlcv_sub(pair.to_string(), interval));

		assert_ohlcv_data_eq(ohlcv_data{}, test.get_last_candle(tradable_pair{"test", "test"}, ohlcv_interval::D1));
	}

	TEST(ExchangeWebsocketStream, SetUnsubscribedClearsCachedOrderBook)
	{
		tradable_pair pair{ "test", "test" };
		mock_exchange_websocket_stream test{ create_mock_stream() };

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

		test.expose_initialise_order_book(pair.to_string(), order_book_cache{1, asks, bids});
		test.expose_set_unsubscribed(named_subscription::create_order_book_sub(pair.to_string()));

		assert_order_book_state_eq(order_book_state{0, {},{} }, test.get_order_book(pair));
	}

	TEST(ExchangeWebsocketStream, SetUnsubscribedSetsUnsubscribedStatus)
	{
		tradable_pair pair{ "test", "test" };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.expose_update_trade(pair.to_string(), trade_update{1, 2.0, 3.0});
		test.expose_set_unsubscribed(named_subscription::create_trade_sub(pair.to_string()));

		EXPECT_EQ(
			subscription_status::UNSUBSCRIBED,
			test.get_subscription_status(unique_websocket_subscription::create_trade_sub(pair)));
	}

	TEST(ExchangeWebsocketStream, UpdateTradeFiresHandler)
	{
		tradable_pair pair{ "test", "test" };
		trade_update trade{ 1, 2.0, 3.0 };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		bool eventFired = false;
		test.set_trade_update_handler([&eventFired](trade_update_message) { eventFired = true; });

		test.subscribe(websocket_subscription::create_trade_sub({ pair }));
		test.expose_update_trade(pair.to_string(), trade);

		ASSERT_TRUE(eventFired);
	}

	TEST(ExchangeWebsocketStream, DoesNotCrashIfEventHandlerNotSet)
	{
		tradable_pair pair{ "test", "test" };
		trade_update trade{ 1, 2.0, 3.0 };
		mock_exchange_websocket_stream test{ create_mock_stream() };

		test.subscribe(websocket_subscription::create_trade_sub({ pair }));

		ASSERT_NO_THROW(test.expose_update_trade(pair.to_string(), trade));
	}
}