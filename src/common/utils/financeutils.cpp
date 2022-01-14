#include "financeutils.h"

namespace cb
{
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

	double calculate_trade_gain(double assetPrice, double tradeValue, double feePercentage, trade_action action)
	{
		int a = static_cast<int>(action);
		return tradeValue * (1 - feePercentage * 0.01) * (a / assetPrice + assetPrice * (1 - a));
	}

	order_book_entry select_entry(const order_book_level& prices, trade_action action)
	{
		return action == trade_action::BUY ? prices.ask() : prices.bid();
	}
}