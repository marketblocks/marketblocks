#pragma once 

#include <gtest/gtest.h>

#include "exchange_test_common.h"
#include "exchanges/exchange.h"

namespace
{
	using namespace mb;

	void execute_add_cancel_order_test(exchange& api, const tradable_pair& pair, double price, order_type orderType, trade_action action, bool cancel = true)
	{
		order_request request = orderType == order_type::LIMIT
			? create_limit_order(pair, action, price, 0.01)
			: create_market_order(pair, action, 0.01);

		std::string orderId;
		ASSERT_NO_THROW(orderId = api.add_order(request));

		if (cancel)
		{
			ASSERT_NO_THROW(api.cancel_order(orderId));
		}
	}
}

namespace mb::test
{
	template<typename Api>
	class ExchangeIntegrationTests : public testing::Test
	{
	protected:
		std::unique_ptr<exchange> _api;
		tradable_pair _testingPair;
		tradable_pair _testingPair2;

		ExchangeIntegrationTests()
			: 
			_api{ mb::create_exchange_api<Api>(true) }, 
			_testingPair{ get_testing_pair<Api>() },
			_testingPair2{ get_testing_pair_2<Api>() }
		{}
	};

	TYPED_TEST_SUITE_P(ExchangeIntegrationTests);

	TYPED_TEST_P(ExchangeIntegrationTests, GetStatus)
	{
		exchange_status status{ this->_api->get_status() };
		ASSERT_EQ(exchange_status::ONLINE, status);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetTradablePairs)
	{
		std::vector<tradable_pair> pairs{ this->_api->get_tradable_pairs() };
		ASSERT_FALSE(pairs.empty());
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetOhlcv)
	{
		std::vector<ohlcv_data> ohlcv{ this->_api->get_ohlcv(this->_testingPair, ohlcv_interval::M15, 5) };
		ASSERT_FALSE(ohlcv.empty());
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetPrice)
	{
		double price{ this->_api->get_price(this->_testingPair) };
		ASSERT_GT(price, 0.0);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetPrices)
	{
		std::unordered_map<tradable_pair, double> prices{ this->_api->get_prices(
			{
				this->_testingPair,
				this->_testingPair2
			}) };

		ASSERT_FALSE(prices.empty());
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetOrderBook)
	{
		order_book_state orderBook{ this->_api->get_order_book(this->_testingPair, 5) };
		ASSERT_GT(orderBook.depth(), 0);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetOrderBooks)
	{
		std::unordered_map<tradable_pair, order_book_state> orderBooks{ this->_api->get_order_books(
			{
				this->_testingPair,
				this->_testingPair2
			},
			5) };

		ASSERT_FALSE(orderBooks.empty());
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetBalances)
	{
		ASSERT_NO_THROW(this->_api->get_balances());
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetFee)
	{
		ASSERT_NO_THROW(this->_api->get_fee(this->_testingPair));
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetOpenOrders)
	{
		ASSERT_NO_THROW(this->_api->get_open_orders());
	}

	TYPED_TEST_P(ExchangeIntegrationTests, GetClosedOrders)
	{
		ASSERT_NO_THROW(this->_api->get_closed_orders());
	}

	TYPED_TEST_P(ExchangeIntegrationTests, AddCancelBuyLimitOrder)
	{
		double price{ std::round(this->_api->get_price(this->_testingPair)) };
		execute_add_cancel_order_test(*this->_api, this->_testingPair, price, order_type::LIMIT, trade_action::BUY);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, AddCancelSellLimitOrder)
	{
		double price{ std::round(this->_api->get_price(this->_testingPair)) };
		execute_add_cancel_order_test(*this->_api, this->_testingPair, price, order_type::LIMIT, trade_action::SELL);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, AddBuyMarketOrder)
	{
		double price{ this->_api->get_price(this->_testingPair) };
		execute_add_cancel_order_test(*this->_api, this->_testingPair, price, order_type::MARKET, trade_action::BUY, false);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, AddSellMarketOrder)
	{
		double price{ this->_api->get_price(this->_testingPair) };
		execute_add_cancel_order_test(*this->_api, this->_testingPair, price, order_type::MARKET, trade_action::SELL, false);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, WebsocketFeedUpdatesTrade)
	{
		auto websocketStream{ this->_api->get_websocket_stream() };
		websocketStream->subscribe(websocket_subscription::create_trade_sub({ this->_testingPair }));

		bool isTradeUpdated = wait_for_condition(
			[websocketStream, this]() { return websocketStream->get_last_trade(this->_testingPair).time_stamp() != 0.0; },
			10000);

		EXPECT_TRUE(isTradeUpdated);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, WebsocketFeedUpdatesOhlcv)
	{
		auto websocketStream{ this->_api->get_websocket_stream() };
		websocketStream->subscribe(websocket_subscription::create_ohlcv_sub({ this->_testingPair }, ohlcv_interval::M5));

		bool isOhlcvUpdated = wait_for_condition(
			[websocketStream, this]()
			{ 
				return websocketStream->get_last_candle(this->_testingPair, ohlcv_interval::M5).time_stamp() > 0.0;
			},
			10000);

		EXPECT_TRUE(isOhlcvUpdated);
	}

	TYPED_TEST_P(ExchangeIntegrationTests, WebsocketFeedUpdatesOrderBook)
	{
		auto websocketStream{ this->_api->get_websocket_stream() };
		websocketStream->subscribe(websocket_subscription::create_order_book_sub({ this->_testingPair }));

		bool isOrderBookUpdated = wait_for_condition(
			[websocketStream, this]()
			{
				return websocketStream->get_order_book(this->_testingPair).depth() > 0;
			},
			10000);

		EXPECT_TRUE(isOrderBookUpdated);
	}

	REGISTER_TYPED_TEST_SUITE_P(ExchangeIntegrationTests,
		GetStatus,
		GetTradablePairs,
		GetOhlcv,
		GetPrice,
		GetPrices,
		GetOrderBook,
		GetOrderBooks,
		GetBalances,
		GetFee,
		GetOpenOrders,
		GetClosedOrders,
		AddCancelBuyLimitOrder,
		AddCancelSellLimitOrder,
		AddBuyMarketOrder,
		AddSellMarketOrder,
		WebsocketFeedUpdatesTrade,
		WebsocketFeedUpdatesOhlcv,
		WebsocketFeedUpdatesOrderBook);
}