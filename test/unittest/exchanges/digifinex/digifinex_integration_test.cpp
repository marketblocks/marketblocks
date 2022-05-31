#include <gtest/gtest.h>

#include "exchanges/digifinex/digifinex.h"

namespace
{
	using namespace mb;

	std::unique_ptr<exchange> create_api()
	{
		std::shared_ptr<websocket_client> websocketClient{ std::make_shared<websocket_client>() };
		return make_digifinex(websocketClient);
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

	TEST(DigifinexIntegration, Get24hStats)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_24h_stats(pair));
	}

	TEST(DigifinexIntegration, GetPrice)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_price(pair));
	}

	TEST(DigifinexIntegration, GetOrderBook)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto digifinex{ create_api() };
		ASSERT_NO_THROW(digifinex->get_order_book(pair, 5));
	}

	TEST(DigifinexIntegration, GetFee)
	{
		tradable_pair pair{ "BTC", "USD" };

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
			tradable_pair{ "BTC", "USD" },
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