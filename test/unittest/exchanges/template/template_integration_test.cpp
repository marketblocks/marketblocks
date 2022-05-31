#include <gtest/gtest.h>

#include "exchanges/template/template.h"

namespace
{
	using namespace mb;

	std::unique_ptr<exchange> create_api()
	{
		std::shared_ptr<websocket_client> websocketClient{ std::make_shared<websocket_client>() };
		return make_template(websocketClient);
	}
}

namespace mb::test
{
	TEST(TemplateIntegration, GetStatus)
	{
		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_status());
	}

	TEST(TemplateIntegration, GetTradablePairs)
	{
		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_tradable_pairs());
	}

	TEST(TemplateIntegration, Get24hStats)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_24h_stats(pair));
	}

	TEST(TemplateIntegration, GetPrice)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_price(pair));
	}

	TEST(TemplateIntegration, GetOrderBook)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_order_book(pair, 5));
	}

	TEST(TemplateIntegration, GetFee)
	{
		tradable_pair pair{ "BTC", "USD" };

		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_fee(pair));
	}

	TEST(TemplateIntegration, GetBalances)
	{
		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_balances());
	}

	TEST(TemplateIntegration, GetOpenOrders)
	{
		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_open_orders());
	}

	TEST(TemplateIntegration, GetClosedOrders)
	{
		auto temp{ create_api() };
		ASSERT_NO_THROW(temp->get_closed_orders());
	}

	TEST(TemplateIntegration, AddCancelOrder)
	{
		trade_description trade
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "USD" },
			trade_action::BUY,
			37500,
			1.25
		};

		auto temp{ create_api() };
		std::string orderId;

		ASSERT_NO_THROW(orderId = temp->add_order(trade));
		ASSERT_NO_THROW(temp->cancel_order(orderId));
	}
}