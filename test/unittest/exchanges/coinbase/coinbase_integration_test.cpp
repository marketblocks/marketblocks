//#include <gtest/gtest.h>
//
//#include "test_data/test_exchange_configs/test_config_loader.h"
//#include "exchanges/coinbase/coinbase.h"
//#include "common/utils/timeutils.h"
//
//namespace
//{
//	using namespace mb;
//	using namespace mb::test;
//
//	std::unique_ptr<exchange> create_api()
//	{
//		coinbase_config config{ load_test_config<coinbase_config>() };
//		return make_coinbase(std::move(config), true);
//	}
//}
//
//namespace mb::test
//{
//	TEST(CoinbaseIntegration, GetStatus)
//	{
//		auto coinbase{ create_api() };
//		ASSERT_NO_THROW(coinbase->get_status());
//	}
//
//	TEST(CoinbaseIntegration, GetTradablePairs)
//	{
//		auto coinbase{ create_api() };
//		ASSERT_NO_THROW(coinbase->get_tradable_pairs());
//	}
//
//	TEST(CoinbaseIntegration, GetPrice)
//	{
//		tradable_pair pair{ "BTC", "USD" };
//
//		auto coinbase{ create_api() };
//		ASSERT_NO_THROW(coinbase->get_price(pair));
//	}
//
//	TEST(CoinbaseIntegration, GetOrderBook)
//	{
//		tradable_pair pair{ "BTC", "USD" };
//
//		auto coinbase{ create_api() };
//		ASSERT_NO_THROW(coinbase->get_order_book(pair, 5));
//	}
//
//	TEST(CoinbaseIntegration, GetFee)
//	{
//		tradable_pair pair{ "BTC", "USD" };
//
//		auto coinbase{ create_api() };
//		ASSERT_NO_THROW(coinbase->get_fee(pair));
//	}
//
//	TEST(CoinbaseIntegration, GetBalances)
//	{
//		auto coinbase{ create_api() };
//		ASSERT_NO_THROW(coinbase->get_balances());
//	}
//
//	TEST(CoinbaseIntegration, GetOpenOrders)
//	{
//		auto coinbase{ create_api() };
//		ASSERT_NO_THROW(coinbase->get_open_orders());
//	}
//
//	TEST(CoinbaseIntegration, GetClosedOrders)
//	{
//		auto coinbase{ create_api() };
//		ASSERT_NO_THROW(coinbase->get_closed_orders());
//	}
//
//	TEST(CoinbaseIntegration, AddCancelLimitOrder)
//	{
//		trade_description trade
//		{
//			order_type::LIMIT,
//			tradable_pair{ "BTC", "USD" },
//			trade_action::BUY,
//			1.0,
//			1.0
//		};
//
//		auto coinbase{ create_api() };
//		std::string orderId;
//
//		ASSERT_NO_THROW(orderId = coinbase->add_order(trade));
//		ASSERT_NO_THROW(coinbase->cancel_order(orderId));
//	}
//
//	TEST(CoinbaseIntegration, AddMarketOrder)
//	{
//		trade_description buyTrade
//		{
//			order_type::MARKET,
//			tradable_pair{ "BTC", "USD" },
//			trade_action::BUY,
//			1.0,
//			1.0
//		};
//
//		trade_description sellTrade
//		{
//			order_type::MARKET,
//			tradable_pair{ "BTC", "USD" },
//			trade_action::SELL,
//			1.0,
//			1.0
//		};
//
//		auto coinbase{ create_api() };
//
//		ASSERT_NO_THROW(coinbase->add_order(buyTrade));
//		ASSERT_NO_THROW(coinbase->add_order(sellTrade));
//	}
//
//	TEST(CoinbaseIntegration, AddCancelStopLossOrder)
//	{
//		trade_description trade
//		{
//			order_type::STOP_LOSS,
//			tradable_pair{ "BTC", "USD" },
//			trade_action::SELL,
//			1.0,
//			1.0
//		};
//
//		auto coinbase{ create_api() };
//		std::string orderId;
//
//		ASSERT_NO_THROW(orderId = coinbase->add_order(trade));
//		ASSERT_NO_THROW(coinbase->cancel_order(orderId));
//	}
//
//	TEST(CoinbaseIntegration, AddCancelTakeProfitOrder)
//	{
//		trade_description trade
//		{
//			order_type::TAKE_PROFIT,
//			tradable_pair{ "BTC", "USD" },
//			trade_action::SELL,
//			1e9,
//			1.0
//		};
//
//		auto coinbase{ create_api() };
//		std::string orderId;
//
//		ASSERT_NO_THROW(orderId = coinbase->add_order(trade));
//		ASSERT_NO_THROW(coinbase->cancel_order(orderId));
//	}
//}