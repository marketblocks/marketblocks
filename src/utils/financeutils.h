#pragma once

#include <misc/trading.h>
#include <misc/data.h>

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

inline double select_price(const PriceData& prices, TradeAction action)
{
	return action == TradeAction::BUY ? prices.ask() : prices.bid();
}