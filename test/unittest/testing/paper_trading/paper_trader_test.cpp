#include <gtest/gtest.h>

#include "testing/paper_trading/paper_trade_api.h"
#include "common/exceptions/mb_exception.h"
#include "common/utils/containerutils.h"

namespace
{
	using namespace mb;

	class get_prices
	{
	private:
		std::unordered_map<tradable_pair, double> _prices;

	public:
		get_prices(std::unordered_map<tradable_pair, double> prices)
			: _prices{ std::move(prices) }
		{}

		void set_price(tradable_pair pair, double price)
		{
			_prices.insert_or_assign(std::move(pair), price);
		}

		double get_price(const tradable_pair& pair) const
		{
			return find_or_default<double>(_prices, pair);
		}
	};

	paper_trade_api create_paper_trade_api(
		double initialGbpBalance,
		double initialBtcBalance,
		double fee,
		const get_prices& getPrices)
	{
		std::unordered_map<std::string,double> initialBalances
		{
			{ "GBP", initialGbpBalance },
			{ "BTC", initialBtcBalance }
		};

		return paper_trade_api{
			paper_trading_config{ fee, std::move(initialBalances) },
			"",
			[&getPrices](const tradable_pair& pair) {return getPrices.get_price(pair); },
			now_t};
	}
}

namespace mb::test
{
	TEST(PaperTrader, AddBuyOrderCorrectlyAdjustsBalances)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 2.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{{ { pair, assetPrice } }};
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_market_order(pair, trade_action::BUY, volume) };

		trader.add_order(orderRequest);

		std::unordered_map<std::string,double> balances = trader.get_balances();
		EXPECT_DOUBLE_EQ(balances.at(orderRequest.pair().asset()), 3.5);
		EXPECT_DOUBLE_EQ(balances.at(orderRequest.pair().price_unit()), 59.96);
	}

	TEST(PaperTrader, AddSellOrderCorrectlyAdjustsBalances)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 1.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_market_order(pair, trade_action::SELL, volume) };

		trader.add_order(orderRequest);

		std::unordered_map<std::string,double> balances = trader.get_balances();
		EXPECT_DOUBLE_EQ(balances.at(orderRequest.pair().asset()), 0.5);
		EXPECT_DOUBLE_EQ(balances.at(orderRequest.pair().price_unit()), 119.98);
	}

	TEST(PaperTrader, AddBuyOrderThrowsIfInsufficientFunds)
	{
		constexpr double initialGbpBalance = 20.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 50.0;
		constexpr double volume = 1.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_market_order(pair, trade_action::BUY, volume) };

		EXPECT_THROW(trader.add_order(orderRequest), mb_exception);
	}

	TEST(PaperTrader, AddSellOrderThrowsIfInsufficientFunds)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 0.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 50.0;
		constexpr double volume = 1.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_market_order(pair, trade_action::SELL, volume) };

		EXPECT_THROW(trader.add_order(orderRequest), mb_exception);
	}

	TEST(PaperTrade, BuyLimitOrderDoesNotExecuteIfPriceGreaterThanLimitPrice)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 40.0;
		constexpr double volume = 2.0;
		constexpr double orderPrice = 20.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_limit_order(pair, trade_action::BUY, orderPrice, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		std::vector<order_description> openOrders{ trader.get_open_orders() };
		ASSERT_TRUE(openOrders.size() == 1);
		ASSERT_EQ(orderId, openOrders.front().order_id());
	}

	TEST(PaperTrade, SellLimitOrderDoesNotExecuteIfPriceLessThanLimitPrice)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 2.0;
		constexpr double orderPrice = 40.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_limit_order(pair, trade_action::SELL, orderPrice, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		std::vector<order_description> openOrders{ trader.get_open_orders() };
		ASSERT_TRUE(openOrders.size() == 1);
		ASSERT_EQ(orderId, openOrders.front().order_id());
	}

	TEST(PaperTrader, BuyLimitOrderExecutesWhenPriceLessThanLimitPrice)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 40.0;
		constexpr double volume = 2.0;
		constexpr double orderPrice = 20.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_limit_order(pair, trade_action::BUY, orderPrice, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		prices.set_price(pair, 19.0);
		trader.try_fill_open_orders();

		std::vector<order_description> closedOrders{ trader.get_closed_orders() };
		ASSERT_TRUE(closedOrders.size() == 1);
		ASSERT_EQ(orderId, closedOrders.front().order_id());
	}

	TEST(PaperTrader, SellLimitOrderExecutesWhenPriceGreaterThanLimitPrice)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 1.0;
		constexpr double orderPrice = 40.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_limit_order(pair, trade_action::SELL, orderPrice, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		prices.set_price(pair, 41.0);
		trader.try_fill_open_orders();

		std::vector<order_description> closedOrders{ trader.get_closed_orders() };
		ASSERT_TRUE(closedOrders.size() == 1);
		ASSERT_EQ(orderId, closedOrders.front().order_id());
	}

	TEST(PaperTrader, StopLossSellOrderDoesNotExecuteWhenPriceGreaterThanStopPrice)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 40.0;
		constexpr double volume = 1.0;
		constexpr double stopPrice = 20.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_stop_loss_order(pair, trade_action::SELL, stopPrice, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		std::vector<order_description> openOrders{ trader.get_open_orders() };
		ASSERT_TRUE(openOrders.size() == 1);
		ASSERT_EQ(orderId, openOrders.front().order_id());
	}

	TEST(PaperTrader, StopLossSellOrderExecutesWhenPriceLessThanStopPrice)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 40.0;
		constexpr double volume = 1.0;
		constexpr double stopPrice = 20.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_stop_loss_order(pair, trade_action::SELL, stopPrice, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		ASSERT_TRUE(trader.get_closed_orders().empty());

		prices.set_price(pair, 19.0);
		trader.try_fill_open_orders();

		std::vector<order_description> closedOrders{ trader.get_closed_orders() };
		ASSERT_TRUE(closedOrders.size() == 1);
		ASSERT_EQ(orderId, closedOrders.front().order_id());
	}

	TEST(PaperTrader, StopLossBuyOrderDoesNotExecuteWhenPriceLessThanStopPrice)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 1.0;
		constexpr double stopPrice = 40.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_stop_loss_order(pair, trade_action::BUY, stopPrice, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		std::vector<order_description> openOrders{ trader.get_open_orders() };
		ASSERT_TRUE(openOrders.size() == 1);
		ASSERT_EQ(orderId, openOrders.front().order_id());
	}

	TEST(PaperTrader, StopLossBuyOrderExecutesWhenPriceGreaterThanStopPrice)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 1.0;
		constexpr double stopPrice = 40.0;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_stop_loss_order(pair, trade_action::BUY, stopPrice, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		ASSERT_TRUE(trader.get_closed_orders().empty());

		prices.set_price(pair, 41.0);
		trader.try_fill_open_orders();

		std::vector<order_description> closedOrders{ trader.get_closed_orders() };
		ASSERT_TRUE(closedOrders.size() == 1);
		ASSERT_EQ(orderId, closedOrders.front().order_id());
	}

	TEST(PaperTrader, TrailingStopLossBuyOrderExecutesWhenPriceGreaterThanMinPlusDelta)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 1.0;
		constexpr double delta = 0.10;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_trailing_stop_loss_order(pair, trade_action::BUY, delta, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		ASSERT_TRUE(trader.get_closed_orders().empty());

		prices.set_price(pair, 21.0);
		trader.try_fill_open_orders();

		ASSERT_TRUE(trader.get_closed_orders().empty());

		prices.set_price(pair, 15.0);
		trader.try_fill_open_orders();

		ASSERT_TRUE(trader.get_closed_orders().empty());

		prices.set_price(pair, 17.0);
		trader.try_fill_open_orders();

		std::vector<order_description> closedOrders{ trader.get_closed_orders() };
		ASSERT_TRUE(closedOrders.size() == 1);
		ASSERT_EQ(orderId, closedOrders.front().order_id());
	}

	TEST(PaperTrader, TrailingStopLossSellOrderExecutesWhenPriceLessThanMaxMinusDelta)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 1.0;
		constexpr double delta = 0.10;

		tradable_pair pair{ "BTC", "GBP" };
		get_prices prices{ { { pair, assetPrice } } };
		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee, prices) };
		order_request orderRequest{ create_trailing_stop_loss_order(pair, trade_action::SELL, delta, volume) };

		std::string orderId{ trader.add_order(orderRequest) };

		ASSERT_TRUE(trader.get_closed_orders().empty());

		prices.set_price(pair, 19.0);
		trader.try_fill_open_orders();

		ASSERT_TRUE(trader.get_closed_orders().empty());

		prices.set_price(pair, 25.0);
		trader.try_fill_open_orders();

		ASSERT_TRUE(trader.get_closed_orders().empty());

		prices.set_price(pair, 22.0);
		trader.try_fill_open_orders();

		std::vector<order_description> closedOrders{ trader.get_closed_orders() };
		ASSERT_TRUE(closedOrders.size() == 1);
		ASSERT_EQ(orderId, closedOrders.front().order_id());
	}
}