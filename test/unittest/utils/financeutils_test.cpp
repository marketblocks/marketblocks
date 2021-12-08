#include <gtest/gtest.h>

#include "utils/financeutils.h"

TEST(FinanceUtils, CalculateCost)
{
	EXPECT_DOUBLE_EQ(calculate_cost(5000, 1), 5000);
	EXPECT_DOUBLE_EQ(calculate_cost(5000, 2), 10000);
	EXPECT_DOUBLE_EQ(calculate_cost(5000, 0.5), 2500);
}

TEST(FinanceUtils, CalculateVolume)
{
	EXPECT_DOUBLE_EQ(calculate_volume(5000, 5000), 1);
	EXPECT_DOUBLE_EQ(calculate_volume(5000, 10000), 2);
	EXPECT_DOUBLE_EQ(calculate_volume(5000, 2500), 0.5);
}

TEST(FinanceUtils, CalculateAssetPrice)
{
	EXPECT_DOUBLE_EQ(calculate_asset_price(5000, 1), 5000);
	EXPECT_DOUBLE_EQ(calculate_asset_price(10000, 2), 5000);
	EXPECT_DOUBLE_EQ(calculate_asset_price(2500, 0.5), 5000);
}

TEST(FinanceUtils, CalculateFee)
{
	EXPECT_DOUBLE_EQ(calculate_fee(100, 1), 1.0);
	EXPECT_DOUBLE_EQ(calculate_fee(50, 0.1), 0.05);
}

TEST(FinanceUtils, CalculateTradeGain)
{
	EXPECT_DOUBLE_EQ(calculate_trade_gain(0.8, 5.8806, 1.0, TradeAction::BUY), 7.2772425);
	EXPECT_DOUBLE_EQ(calculate_trade_gain(2.55, 1.9602, 1.0, TradeAction::SELL), 4.9485249);
}

TEST(FinanceUtils, SelectPrice)
{
	OrderBookLevel level{ OrderBookEntry {1.0, 0.0}, OrderBookEntry{2.0, 0.0} };

	EXPECT_DOUBLE_EQ(select_entry(level, TradeAction::BUY).price(), level.ask().price());
	EXPECT_DOUBLE_EQ(select_entry(level, TradeAction::SELL).price(), level.bid().price());
}