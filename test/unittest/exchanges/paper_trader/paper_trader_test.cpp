#include <gtest/gtest.h>

#include "exchanges/paper_trading/paper_trader.h"

TEST(PaperTrader, TradeBuy)
{
	constexpr double initialGbpBalance = 100.0;
	constexpr double initialBtcBalance = 1.5;

	std::unordered_map<AssetSymbol, double> initialBalances
	{
		{ AssetSymbol{ "GBP" }, initialGbpBalance },
		{ AssetSymbol{ "BTC" }, initialBtcBalance }
	};

	FeeSchedule fees = FeeScheduleBuilder{}
		.add_tier(1000, 0.1)
		.build();

	PaperTrader trader{ fees, initialBalances };
	TradablePair pair{ AssetSymbol{ "BTC" }, AssetSymbol{ "GBP" } };
	TradeDescription tradeDescription
	{
		pair,
		TradeAction::BUY,
		20.0,
		2.0
	};

	constexpr double cost = 40.04;
	constexpr double expectedGbpBalance = initialGbpBalance - cost;
	constexpr double expectedBtcBalance = initialBtcBalance + 2.0;

	trader.trade(tradeDescription);

	std::unordered_map<AssetSymbol, double> balances = trader.get_balances();

	EXPECT_DOUBLE_EQ(balances.at(pair.asset()), expectedBtcBalance);
	EXPECT_DOUBLE_EQ(balances.at(pair.price_unit()), expectedGbpBalance);
}

TEST(PaperTrader, TradeSell)
{
	constexpr double initialGbpBalance = 100.0;
	constexpr double initialBtcBalance = 1.5;

	std::unordered_map<AssetSymbol, double> initialBalances
	{
		{ AssetSymbol{ "GBP" }, initialGbpBalance },
		{ AssetSymbol{ "BTC" }, initialBtcBalance }
	};

	FeeSchedule fees = FeeScheduleBuilder{}
		.add_tier(1000, 0.1)
		.build();

	PaperTrader trader{ fees, initialBalances };
	TradablePair pair{ AssetSymbol{ "BTC" }, AssetSymbol{ "GBP" } };
	TradeDescription tradeDescription
	{
		pair,
		TradeAction::SELL,
		20.0,
		1.0
	};

	constexpr double saleReturn = 19.98;
	constexpr double expectedGbpBalance = initialGbpBalance + saleReturn;
	constexpr double expectedBtcBalance = initialBtcBalance - 1.0;

	trader.trade(tradeDescription);

	std::unordered_map<AssetSymbol, double> balances = trader.get_balances();

	EXPECT_DOUBLE_EQ(balances.at(pair.asset()), expectedBtcBalance);
	EXPECT_DOUBLE_EQ(balances.at(pair.price_unit()), expectedGbpBalance);
}