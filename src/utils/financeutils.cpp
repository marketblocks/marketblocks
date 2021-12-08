#include "financeutils.h"

double calculate_cost(double assetPrice, double volume)
{
	return assetPrice * volume;
}

double calculate_volume(double assetPrice, double cost)
{
	return cost / assetPrice;
}

double calculate_asset_price(double cost, double volume)
{
	return cost / volume;
}

double calculate_fee(double amount, double feePercentage)
{
	return amount * feePercentage * 0.01;
}

double calculate_trade_gain(double assetPrice, double tradeValue, double feePercentage, TradeAction action)
{
	int a = static_cast<int>(action);
	return tradeValue * (1 - feePercentage * 0.01) * (a / assetPrice + assetPrice * (1 - a));
}

double select_price(const PriceData& prices, TradeAction action)
{
	return action == TradeAction::BUY ? prices.ask() : prices.bid();
}