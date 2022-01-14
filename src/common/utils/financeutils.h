#pragma once

#include "trading/trading_constants.h"
#include "trading/order_book.h"

namespace cb
{
	double calculate_cost(double assetPrice, double volume);

	double calculate_volume(double assetPrice, double cost);

	double calculate_asset_price(double cost, double volume);

	double calculate_fee(double amount, double feePercentage);

	double calculate_trade_gain(double assetPrice, double tradeValue, double feePercentage, trade_action action);

	order_book_entry select_entry(const order_book_level& prices, trade_action action);
}