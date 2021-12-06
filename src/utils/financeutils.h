#pragma once

#include "common/trading/trading_constants.h"
#include "common/data/data.h"

inline double calculate_cost(double assetPrice, double volume)
{
	return assetPrice * volume;
}

inline double calculate_volume(double assetPrice, double cost)
{
	return cost / assetPrice;
}

inline double calculate_asset_price(double cost, double volume)
{
	return cost / volume;
}

inline double calculate_fee(double amount, double feePercentage)
{
	return amount * feePercentage * 0.01;
}

inline double select_price(const PriceData& prices, TradeAction action)
{
	return action == TradeAction::BUY ? prices.ask() : prices.bid();
}