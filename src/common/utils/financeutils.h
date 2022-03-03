#pragma once

#include "trading/trading_constants.h"
#include "trading/order_book.h"

namespace cb
{
	constexpr double calculate_cost(double assetPrice, double volume) noexcept
	{
		return assetPrice * volume;
	}

	constexpr double calculate_volume(double assetPrice, double cost) noexcept
	{
		return cost / assetPrice;
	}

	constexpr double calculate_asset_price(double cost, double volume) noexcept
	{
		return cost / volume;
	}

	constexpr double calculate_fee(double amount, double feePercentage) noexcept
	{
		return amount * feePercentage * 0.01;
	}

	constexpr double calculate_trade_gain(double assetPrice, double tradeValue, double feePercentage, trade_action action) noexcept
	{
		int a = static_cast<int>(action);
		return tradeValue * (1 - feePercentage * 0.01) * (a / assetPrice + assetPrice * (1 - a));
	}

	constexpr order_book_entry select_entry(const order_book_level& prices, trade_action action) noexcept
	{
		return action == trade_action::BUY ? prices.ask() : prices.bid();
	}
}