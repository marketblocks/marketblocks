#include <gtest/gtest.h>

#include "exchanges/paper_trading/paper_trader.h"

namespace
{
	struct PaperTraderTestComponents
	{
		cb::paper_trader trader;
		cb::trade_description tradeDescription;
	};

	PaperTraderTestComponents setup_gbp_btc_transaction_test(
		double initialGbpBalance,
		double initialBtcBalance,
		cb::trade_action tradeAction,
		double assetPrice,
		double tradeVolume,
		double fee)
	{
		cb::unordered_string_map<double> initialBalances
		{
			{ "GBP", initialGbpBalance },
			{ "BTC", initialBtcBalance }
		};

		cb::fee_schedule fees = cb::fee_schedule_builder{}
			.add_tier(1000, fee)
			.build();

		cb::paper_trader trader{ fees, initialBalances };
		cb::tradable_pair pair{ "BTC", "GBP" };
		cb::trade_description tradeDescription
		{
			cb::order_type::LIMIT,
			pair,
			tradeAction,
			assetPrice,
			tradeVolume
		};

		return { std::move(trader), std::move(tradeDescription) };
	}
}

namespace cb::test
{
	TEST(paper_trader, TradeBuy)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;

		auto [trader, tradeDescription] = setup_gbp_btc_transaction_test(
			initialGbpBalance,
			initialBtcBalance,
			trade_action::BUY,
			20.0,
			2.0,
			0.1);

		constexpr double expectedGbpBalance = initialGbpBalance - 40.04;
		constexpr double expectedBtcBalance = initialBtcBalance + 2.0;

		std::string result = trader.add_order(tradeDescription);

		//ASSERT_EQ(result, trade_result::SUCCESS);

		unordered_string_map<double> balances = trader.get_balances();

		EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().asset()), expectedBtcBalance);
		EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().price_unit()), expectedGbpBalance);
	}

	TEST(paper_trader, TradeSell)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 1.5;

		auto [trader, tradeDescription] = setup_gbp_btc_transaction_test(
			initialGbpBalance,
			initialBtcBalance,
			trade_action::SELL,
			20.0,
			1.0,
			0.1);

		constexpr double expectedGbpBalance = initialGbpBalance + 19.98;
		constexpr double expectedBtcBalance = initialBtcBalance - 1.0;

		std::string result = trader.add_order(tradeDescription);

		//ASSERT_EQ(result, trade_result::SUCCESS);

		unordered_string_map<double> balances = trader.get_balances();

		EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().asset()), expectedBtcBalance);
		EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().price_unit()), expectedGbpBalance);
	}

	TEST(paper_trader, TradeBuyInsufficientFunds)
	{
		constexpr double initialGbpBalance = 20.0;
		constexpr double initialBtcBalance = 1.5;

		auto [trader, tradeDescription] = setup_gbp_btc_transaction_test(
			initialGbpBalance,
			initialBtcBalance,
			trade_action::BUY,
			20.0,
			2.0,
			0.1);

		std::string result = trader.add_order(tradeDescription);

		//ASSERT_EQ(result, trade_result::INSUFFICENT_FUNDS);
	}

	TEST(paper_trader, TradeSellInsufficientFunds)
	{
		constexpr double initialGbpBalance = 100.0;
		constexpr double initialBtcBalance = 0.5;

		auto [trader, tradeDescription] = setup_gbp_btc_transaction_test(
			initialGbpBalance,
			initialBtcBalance,
			trade_action::SELL,
			20.0,
			1.0,
			0.1);

		std::string result = trader.add_order(tradeDescription);

		//ASSERT_EQ(result, trade_result::INSUFFICENT_FUNDS);
	}
}