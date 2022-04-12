#include <gtest/gtest.h>

#include "test_data/test_exchange_configs/test_config_loader.h"
#include "exchanges/kraken/kraken.h"

namespace
{
	using namespace mb;

	std::unique_ptr<exchange> create_api()
	{
		kraken_config config{ test::load_test_config<kraken_config>() };
		std::shared_ptr<websocket_client> websocketClient{ std::make_shared<websocket_client>() };
		return make_kraken(std::move(config), websocketClient);
	}
}

namespace mb::test
{
	TEST(KrakenIntegration, GetStatus)
	{
		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_status());
	}

	TEST(KrakenIntegration, GetTradablePairs)
	{
		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_tradable_pairs());
	}

	TEST(KrakenIntegration, Get24hStats)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_24h_stats(pair));
	}

	TEST(KrakenIntegration, GetPrice)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_price(pair));
	}

	TEST(KrakenIntegration, GetOrderBook)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_order_book(pair, 5));
	}

	TEST(KrakenIntegration, GetFee)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_fee(pair));
	}

	TEST(KrakenIntegration, GetBalances)
	{
		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_balances());
	}

	TEST(KrakenIntegration, GetOpenOrders)
	{
		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_open_orders());
	}

	TEST(KrakenIntegration, GetClosedOrders)
	{
		auto kraken{ create_api() };
		ASSERT_NO_THROW(kraken->get_closed_orders());
	}

	TEST(KrakenIntegration, AddCancelOrder)
	{
		trade_description trade
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "USD" },
			trade_action::BUY,
			37500,
			1.25
		};

		auto kraken{ create_api() };
		std::string orderId;

		ASSERT_NO_THROW(orderId = kraken->add_order(trade));
		ASSERT_NO_THROW(kraken->cancel_order(orderId));
	}
}