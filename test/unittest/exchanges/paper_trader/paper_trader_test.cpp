#include <gtest/gtest.h>

#include "testing/paper_trading/paper_trade_api.h"
#include "common/exceptions/mb_exception.h"

namespace
{
	using namespace mb;

	paper_trade_api create_paper_trade_api(
		double initialGbpBalance,
		double initialBtcBalance,
		double fee)
	{
		unordered_string_map<double> initialBalances
		{
			{ "GBP", initialGbpBalance },
			{ "BTC", initialBtcBalance }
		};

		return paper_trade_api{ paper_trading_config{ fee, std::move(initialBalances) } };
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

		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee) };

		trade_description tradeDescription
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "GBP" },
			trade_action::BUY,
			assetPrice,
			volume
		};

		constexpr double expectedGbpBalance = 59.96;
		constexpr double expectedBtcBalance = 3.5;

		std::string orderId = trader.add_order(tradeDescription);

		unordered_string_map<double> balances = trader.get_balances();

		EXPECT_EQ(orderId, "1");
		EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().asset()), expectedBtcBalance);
		EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().price_unit()), expectedGbpBalance);
	}

	TEST(PaperTrader, AddSellOrderCorrectlyAdjustsBalances)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 20.0;
		constexpr double volume = 1.0;

		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee) };

		trade_description tradeDescription
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "GBP" },
			trade_action::SELL,
			assetPrice,
			volume
		};

		constexpr double expectedGbpBalance = 119.98;
		constexpr double expectedBtcBalance = 0.5;

		std::string orderId = trader.add_order(tradeDescription);

		unordered_string_map<double> balances = trader.get_balances();

		EXPECT_EQ(orderId, "1");
		EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().asset()), expectedBtcBalance);
		EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().price_unit()), expectedGbpBalance);
	}

	TEST(PaperTrader, AddBuyOrderThrowsIfInsufficientFunds)
	{
		constexpr double initialGbpBalance = 20.0;
		constexpr double initialBtcBalance = 1.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 50.0;
		constexpr double volume = 1.0;

		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee) };

		trade_description tradeDescription
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "GBP" },
			trade_action::BUY,
			assetPrice,
			volume
		};

		EXPECT_THROW(trader.add_order(tradeDescription), mb_exception);
	}

	TEST(PaperTrader, AddSellOrderThrowsIfInsufficientFunds)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 0.5;
		constexpr double fee = 0.1;
		constexpr double assetPrice = 50.0;
		constexpr double volume = 1.0;

		paper_trade_api trader{ create_paper_trade_api(initialGbpBalance, initialBtcBalance, fee) };

		trade_description tradeDescription
		{
			order_type::LIMIT,
			tradable_pair{ "BTC", "GBP" },
			trade_action::SELL,
			assetPrice,
			volume
		};

		EXPECT_THROW(trader.add_order(tradeDescription), mb_exception);
	}
}