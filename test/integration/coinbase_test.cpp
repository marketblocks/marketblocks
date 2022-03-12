#include <gtest/gtest.h>

#include "test_data/test_exchange_configs/test_config_loader.h"
#include "exchanges/coinbase/coinbase.h"

namespace
{
	std::unique_ptr<cb::exchange> create_api()
	{
		cb::coinbase_config config{ cb::test::load_test_config<cb::coinbase_config>() };
		std::shared_ptr<cb::websocket_client> websocketClient{ std::make_shared<cb::websocket_client>() };
		return cb::make_coinbase(std::move(config), websocketClient, true);
	}
}

namespace cb::test
{
	TEST(CoinbaseIntegration, GetStatus)
	{
		auto coinbase{ create_api() };
		ASSERT_NO_THROW(coinbase->get_status());
	}

	TEST(CoinbaseIntegration, GetTradablePairs)
	{
		auto coinbase{ create_api() };
		ASSERT_NO_THROW(coinbase->get_tradable_pairs());
	}

	TEST(CoinbaseIntegration, Get24hStats)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto coinbase{ create_api() };
		ASSERT_NO_THROW(coinbase->get_24h_stats(pair));
	}

	TEST(CoinbaseIntegration, GetOrderBook)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto coinbase{ create_api() };
		ASSERT_NO_THROW(coinbase->get_order_book(pair, 5));
	}

	TEST(CoinbaseIntegration, GetFee)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto coinbase{ create_api() };
		ASSERT_NO_THROW(coinbase->get_fee(pair));
	}

	TEST(CoinbaseIntegration, GetBalances)
	{
		auto coinbase{ create_api() };
		ASSERT_NO_THROW(coinbase->get_balances());
	}

	TEST(CoinbaseIntegration, GetOpenOrders)
	{
		auto coinbase{ create_api() };
		ASSERT_NO_THROW(coinbase->get_open_orders());
	}

	TEST(CoinbaseIntegration, GetClosedOrders)
	{
		auto coinbase{ create_api() };
		ASSERT_NO_THROW(coinbase->get_closed_orders());
	}

	TEST(CoinbaseIntegration, AddCancelOrder)
	{
		trade_description trade
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "USD" },
			trade_action::BUY,
			1.0,
			1.0
		};

		auto coinbase{ create_api() };
		std::string orderId;

		ASSERT_NO_THROW(orderId = coinbase->add_order(trade));
		ASSERT_NO_THROW(coinbase->cancel_order(orderId));
	}
}