#include <gtest/gtest.h>

#include "test_data/test_exchange_configs/test_config_loader.h"
#include "exchanges/bybit/bybit.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	std::unique_ptr<exchange> create_api()
	{
		bybit_config config{ load_test_config<bybit_config>() };
		return make_bybit(std::move(config));
	}
}

namespace mb::test
{
	TEST(ByBitIntegration, GetStatus)
	{
		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_status());
	}

	TEST(ByBitIntegration, GetTradablePairs)
	{
		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_tradable_pairs());
	}

	TEST(ByBitIntegration, Get24hStats)
	{
		tradable_pair pair{ "BTC", "USDT" };

		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_24h_stats(pair));
	}

	TEST(ByBitIntegration, GetPrice)
	{
		tradable_pair pair{ "BTC", "USDT" };

		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_price(pair));
	}

	TEST(ByBitIntegration, GetOrderBook)
	{
		tradable_pair pair{ "BTC", "USDT" };

		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_order_book(pair, 5));
	}

	TEST(ByBitIntegration, GetFee)
	{
		tradable_pair pair{ "BTC", "USDT" };

		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_fee(pair));
	}

	TEST(ByBitIntegration, GetBalances)
	{
		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_balances());
	}

	TEST(ByBitIntegration, GetOpenOrders)
	{
		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_open_orders());
	}

	TEST(ByBitIntegration, GetClosedOrders)
	{
		auto bybit{ create_api() };
		ASSERT_NO_THROW(bybit->get_closed_orders());
	}

	TEST(ByBitIntegration, AddCancelOrder)
	{
		trade_description trade
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "USDT" },
			trade_action::BUY,
			100,
			0.1
		};

		auto bybit{ create_api() };
		std::string orderId;

		ASSERT_NO_THROW(orderId = bybit->add_order(trade));
		ASSERT_NO_THROW(bybit->cancel_order(orderId));
	}
}