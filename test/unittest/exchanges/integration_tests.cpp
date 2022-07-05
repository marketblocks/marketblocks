#include <gtest/gtest.h>

#include "exchange_test_common.h"
#include "exchanges/exchange.h"

namespace
{
	using namespace mb;

	void execute_add_cancel_order_test(exchange& api, order_type orderType, trade_action action, bool cancel = true)
	{
		tradable_pair pair{ api.get_tradable_pairs().front() };
		double price{ api.get_price(pair) };
		trade_description trade{ orderType, pair, action, price, 1.0 };

		std::string orderId;
		ASSERT_NO_THROW(orderId = api.add_order(trade));

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

		ExchangeIntegrationTests()
			: _api{ create_exchange_api<Api>(true) }
		{}
	};

	TYPED_TEST_SUITE(ExchangeIntegrationTests, ExchangeImplementations);

	TYPED_TEST(ExchangeIntegrationTests, GetStatus)
	{
		exchange_status status{ this->_api->get_status() };
		ASSERT_EQ(exchange_status::ONLINE, status);
	}

	TYPED_TEST(ExchangeIntegrationTests, GetTradablePairs)
	{
		std::vector<tradable_pair> pairs{ this->_api->get_tradable_pairs() };
		ASSERT_FALSE(pairs.empty());
	}

	TYPED_TEST(ExchangeIntegrationTests, GetOhlcv)
	{
		tradable_pair pair{ this->_api->get_tradable_pairs().front() };
		std::vector<ohlcv_data> ohlcv{ this->_api->get_ohlcv(pair, ohlcv_interval::M15, 5) };
		ASSERT_FALSE(ohlcv.empty());
	}

	TYPED_TEST(ExchangeIntegrationTests, GetPrice)
	{
		tradable_pair pair{ this->_api->get_tradable_pairs().front() };
		double price{ this->_api->get_price(pair) };
		ASSERT_GT(price, 0.0);
	}

	TYPED_TEST(ExchangeIntegrationTests, GetOrderBook)
	{
		tradable_pair pair{ this->_api->get_tradable_pairs().front() };
		order_book_state orderBook{ this->_api->get_order_book(pair, 5) };
		ASSERT_GT(orderBook.depth(), 0);
	}

	TYPED_TEST(ExchangeIntegrationTests, GetBalances)
	{
		ASSERT_NO_THROW(this->_api->get_balances());
	}

	TYPED_TEST(ExchangeIntegrationTests, GetFee)
	{
		tradable_pair pair{ this->_api->get_tradable_pairs().front() };
		ASSERT_NO_THROW(this->_api->get_fee(pair));
	}

	TYPED_TEST(ExchangeIntegrationTests, GetOpenOrders)
	{
		ASSERT_NO_THROW(this->_api->get_open_orders());
	}

	TYPED_TEST(ExchangeIntegrationTests, GetClosedOrders)
	{
		ASSERT_NO_THROW(this->_api->get_closed_orders());
	}

	TYPED_TEST(ExchangeIntegrationTests, AddCancelBuyLimitOrder)
	{
		execute_add_cancel_order_test(*this->_api, order_type::LIMIT, trade_action::BUY);
	}

	TYPED_TEST(ExchangeIntegrationTests, AddCancelSellLimitOrder)
	{
		execute_add_cancel_order_test(*this->_api, order_type::LIMIT, trade_action::SELL);
	}

	TYPED_TEST(ExchangeIntegrationTests, AddBuyMarketOrder)
	{
		execute_add_cancel_order_test(*this->_api, order_type::MARKET, trade_action::BUY, false);
	}

	TYPED_TEST(ExchangeIntegrationTests, AddSellMarketOrder)
	{
		execute_add_cancel_order_test(*this->_api, order_type::MARKET, trade_action::SELL, false);
	}
}