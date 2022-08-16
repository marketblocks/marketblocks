#include <gtest/gtest.h>

#include "testing/back_testing/backtest_websocket_stream.h"

namespace mb::test
{
	TEST(BackTestWebsocketStream, CallingSubscribeSetsSubscribedStatusForTradeSub)
	{
		backtest_websocket_stream stream{ nullptr };

		tradable_pair pair1{ "BTC", "USD" };
		tradable_pair pair2{ "ETH", "USD" };

		websocket_subscription subscription{ websocket_subscription::create_trade_sub(
			{
				pair1, pair2
			}) };

		stream.subscribe(subscription);

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			stream.get_subscription_status(unique_websocket_subscription::create_trade_sub(pair1)));

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			stream.get_subscription_status(unique_websocket_subscription::create_trade_sub(pair2)));
	}

	TEST(BackTestWebsocketStream, CallingSubscribeSetsSubscribedStatusForOhlcvSub)
	{
		backtest_websocket_stream stream{ nullptr };

		tradable_pair pair1{ "BTC", "USD" };
		tradable_pair pair2{ "ETH", "USD" };

		websocket_subscription subscription{ websocket_subscription::create_ohlcv_sub(
			{
			   pair1, pair2
			}, ohlcv_interval::M5) };

		stream.subscribe(subscription);

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			stream.get_subscription_status(unique_websocket_subscription::create_ohlcv_sub(pair1, ohlcv_interval::M5)));

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			stream.get_subscription_status(unique_websocket_subscription::create_ohlcv_sub(pair2, ohlcv_interval::M5)));
	}

	TEST(BackTestWebsocketStream, CallingSubscribeSetsSubscribedStatusForOrderBookSub)
	{
		backtest_websocket_stream stream{ nullptr };

		tradable_pair pair1{ "BTC", "USD" };
		tradable_pair pair2{ "ETH", "USD" };

		websocket_subscription subscription{ websocket_subscription::create_order_book_sub(
			{
			   pair1, pair2
			}) };

		stream.subscribe(subscription);

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			stream.get_subscription_status(unique_websocket_subscription::create_order_book_sub(pair1)));

		EXPECT_EQ(
			subscription_status::SUBSCRIBED,
			stream.get_subscription_status(unique_websocket_subscription::create_order_book_sub(pair2)));
	}

	TEST(BackTestWebsocketStream, OhlcvSubscriptionsAreAddedWithInterval)
	{
		backtest_websocket_stream stream{ nullptr };

		tradable_pair pair{ "BTC", "USD" };

		websocket_subscription subscription{ websocket_subscription::create_ohlcv_sub(
			{
			   pair
			}, ohlcv_interval::M5) };

		stream.subscribe(subscription);

		EXPECT_EQ(
			subscription_status::UNSUBSCRIBED,
			stream.get_subscription_status(unique_websocket_subscription::create_ohlcv_sub(pair, ohlcv_interval::M1)));
	}

	TEST(BackTestWebsocketStream, CallingUnsubscribeSetsUnsubscribedStatus)
	{
		backtest_websocket_stream stream{ nullptr };

		tradable_pair pair{ "BTC", "USD" };

		websocket_subscription subscription{ websocket_subscription::create_trade_sub(
			{
				pair
			}) };

		stream.subscribe(subscription);
		stream.unsubscribe(subscription);

		EXPECT_EQ(
			subscription_status::UNSUBSCRIBED,
			stream.get_subscription_status(unique_websocket_subscription::create_trade_sub(pair)));
	}

	TEST(BackTestWebsocketStream, NotifyFiresSetUpdateHandlersForAllSusbcriptions)
	{
		backtest_websocket_stream stream{ std::make_shared<back_testing_data>(
			std::vector<tradable_pair>{},
			std::unordered_map<tradable_pair, std::vector<ohlcv_data>>{},
			0, 0, 0, 0) };

		tradable_pair pair{ "BTC", "USD" };

		stream.subscribe(websocket_subscription::create_trade_sub({	pair }));
		stream.subscribe(websocket_subscription::create_ohlcv_sub({	pair }, ohlcv_interval::M5));
		stream.subscribe(websocket_subscription::create_order_book_sub({ pair }));
	
		bool tradeHandlerCalled = false;
		bool ohlcvHandlerCalled = false;
		bool orderBookHandlerCalled = false;

		stream.add_trade_update_handler([&tradeHandlerCalled](trade_update_message) { tradeHandlerCalled = true; });
		stream.add_ohlcv_update_handler([&ohlcvHandlerCalled](ohlcv_update_message) { ohlcvHandlerCalled = true; });
		stream.add_order_book_update_handler([&orderBookHandlerCalled](order_book_update_message) { orderBookHandlerCalled = true; });

		stream.notify();

		EXPECT_TRUE(tradeHandlerCalled);
		EXPECT_TRUE(ohlcvHandlerCalled);
		EXPECT_TRUE(orderBookHandlerCalled);
	}
}