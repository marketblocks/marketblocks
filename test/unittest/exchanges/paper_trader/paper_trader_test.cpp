#include <gtest/gtest.h>

#include "exchanges/paper_trader/paper_trader.h"

TEST(PaperTrader, TradeBuy)
{
	std::unordered_map<std::string, double> initialBalances
	{
		{ "GBP", 100 },
		{ "BTC", 1.5 }
	};

	PaperTrader trader{ 0, initialBalances };
	TradablePair pair{ "BTC", "GBP" };
	TradeDescription tradeDescription
	{
		pair,
		TradeAction::BUY,
		20.0,
		2.0
	};

	trader.trade(tradeDescription);

	EXPECT_DOUBLE_EQ(trader.get_balance(pair.asset()), 3.5);
	EXPECT_DOUBLE_EQ(trader.get_balance(pair.price_unit()), 60.0);
}

TEST(PaperTrader, TradeSell)
{
	std::unordered_map<std::string, double> initialBalances
	{
		{ "GBP", 100 },
		{ "BTC", 1.5 }
	};

	PaperTrader trader{ 0, initialBalances };
	TradablePair pair{ "BTC", "GBP" };
	TradeDescription tradeDescription
	{
		pair,
		TradeAction::SELL,
		20.0,
		1.0
	};

	trader.trade(tradeDescription);

	EXPECT_DOUBLE_EQ(trader.get_balance(pair.asset()), 0.5);
	EXPECT_DOUBLE_EQ(trader.get_balance(pair.price_unit()), 120.0);
}