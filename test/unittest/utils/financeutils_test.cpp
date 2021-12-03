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

TEST(FinanceUtils, SelectPrice)
{
	PriceData prices{ 1.0, 2.0 };

	EXPECT_DOUBLE_EQ(select_price(prices, TradeAction::BUY), prices.ask());
	EXPECT_DOUBLE_EQ(select_price(prices, TradeAction::SELL), prices.bid());
}