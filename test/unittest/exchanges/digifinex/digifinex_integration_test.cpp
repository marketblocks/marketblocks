#include <gtest/gtest.h>

#include "test_data/test_exchange_configs/test_config_loader.h"
#include "exchanges/digifinex/digifinex.h"

namespace
{
	using namespace mb;
	using namespace mb::test;

	std::unique_ptr<exchange> create_api()
	{
		digifinex_config config{ load_test_config<digifinex_config>() };
		return make_digifinex(std::move(config));
	}
}

namespace mb::test
{
	TEST(DigifinexIntegration, GetStatus)
	{
		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_status());
	}

	TEST(DigifinexIntegration, GetTradablePairs)
	{
		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_tradable_pairs());
	}

	TEST(DigifinexIntegration, GetPrice)
	{
		tradable_pair pair{ "BTC", "USDT" };

		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_price(pair));
	}

	TEST(DigifinexIntegration, GetOrderBook)
	{
		tradable_pair pair{ "BTC", "USDT" };

		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_order_book(pair, 5));
	}

	TEST(DigifinexIntegration, GetFee)
	{
		tradable_pair pair{ "BTC", "USDT" };

		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_fee(pair));
	}

	TEST(DigifinexIntegration, GetBalances)
	{
		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_balances());
	}

	TEST(DigifinexIntegration, GetOpenOrders)
	{
		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_open_orders());
	}

	TEST(DigifinexIntegration, GetClosedOrders)
	{
		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_closed_orders());
	}

	TEST(DigifinexIntegration, AddCancelOrder)
	{
		trade_description trade
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "USDT" },
			trade_action::BUY,
			37500,
			1.25
		};

		auto digifinex{ create_api() };
		std::string orderId;

		ASSERT_NO_THROW(orderId = digifinex->add_order(trade));
		ASSERT_NO_THROW(digifinex->cancel_order(orderId));
	}
}