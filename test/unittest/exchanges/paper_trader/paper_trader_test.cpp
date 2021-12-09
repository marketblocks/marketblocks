#include <gtest/gtest.h>

#include "exchanges/paper_trading/paper_trader.h"

namespace
{
	struct PaperTraderTestComponents
	{
		PaperTrader trader;
		TradeDescription tradeDescription;
	};

	PaperTraderTestComponents setup_gbp_btc_transaction_test(
		double initialGbpBalance,
		double initialBtcBalance,
		TradeAction tradeAction,
		double assetPrice,
		double tradeVolume,
		double fee)
	{
		std::unordered_map<AssetSymbol, double> initialBalances
		{
			{ AssetSymbol{ "GBP" }, initialGbpBalance },
			{ AssetSymbol{ "BTC" }, initialBtcBalance }
		};

		FeeSchedule fees = FeeScheduleBuilder{}
			.add_tier(1000, fee)
			.build();

		PaperTrader trader{ fees, initialBalances };
		TradablePair pair{ AssetSymbol{ "BTC" }, AssetSymbol{ "GBP" } };
		TradeDescription tradeDescription
		{
			pair,
			tradeAction,
			assetPrice,
			tradeVolume
		};

		return { std::move(trader), std::move(tradeDescription) };
	}
}

TEST(PaperTrader, TradeBuy)
{
	constexpr double initialGbpBalance = 100.0;
	constexpr double initialBtcBalance = 1.5;

	auto [trader, tradeDescription] = setup_gbp_btc_transaction_test(
		initialGbpBalance, 
		initialBtcBalance,
		TradeAction::BUY,
		20.0,
		2.0,
		0.1);

	constexpr double expectedGbpBalance = initialGbpBalance - 40.04;
	constexpr double expectedBtcBalance = initialBtcBalance + 2.0;

	TradeResult result = trader.trade(tradeDescription);

	ASSERT_EQ(result, TradeResult::SUCCESS);

	std::unordered_map<AssetSymbol, double> balances = trader.get_balances();

	EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().asset()), expectedBtcBalance);
	EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().price_unit()), expectedGbpBalance);
}

TEST(PaperTrader, TradeSell)
{
	constexpr double initialGbpBalance = 100.0;
	constexpr double initialBtcBalance = 1.5;

	auto [trader, tradeDescription] = setup_gbp_btc_transaction_test(
		initialGbpBalance,
		initialBtcBalance,
		TradeAction::SELL,
		20.0,
		1.0,
		0.1);

	constexpr double expectedGbpBalance = initialGbpBalance + 19.98;
	constexpr double expectedBtcBalance = initialBtcBalance - 1.0;

	TradeResult result = trader.trade(tradeDescription);

	ASSERT_EQ(result, TradeResult::SUCCESS);

	std::unordered_map<AssetSymbol, double> balances = trader.get_balances();

	EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().asset()), expectedBtcBalance);
	EXPECT_DOUBLE_EQ(balances.at(tradeDescription.pair().price_unit()), expectedGbpBalance);
}

TEST(PaperTrader, TradeBuyInsufficientFunds)
{
	constexpr double initialGbpBalance = 20.0;
	constexpr double initialBtcBalance = 1.5;

	auto [trader, tradeDescription] = setup_gbp_btc_transaction_test(
		initialGbpBalance,
		initialBtcBalance,
		TradeAction::BUY,
		20.0,
		2.0,
		0.1);

	TradeResult result = trader.trade(tradeDescription);

	ASSERT_EQ(result, TradeResult::INSUFFICENT_FUNDS);
}

TEST(PaperTrader, TradeSellInsufficientFunds)
{
	constexpr double initialGbpBalance = 100.0;
	constexpr double initialBtcBalance = 0.5;

	auto [trader, tradeDescription] = setup_gbp_btc_transaction_test(
		initialGbpBalance,
		initialBtcBalance,
		TradeAction::SELL,
		20.0,
		1.0,
		0.1);

	TradeResult result = trader.trade(tradeDescription);

	ASSERT_EQ(result, TradeResult::INSUFFICENT_FUNDS);
}