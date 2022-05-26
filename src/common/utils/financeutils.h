#pragma once

#include "trading/trading_constants.h"
#include "trading/order_book.h"

namespace mb
{
	constexpr double calculate_cost(double assetPrice, double volume) noexcept
	{
		return assetPrice * volume;
	}

	constexpr double calculate_volume(double assetPrice, double cost) noexcept
	{
		if (cost == 0.0 || assetPrice == 0.0)
		{
			return 0.0;
		}

		return cost / assetPrice;
	}

	constexpr double calculate_asset_price(double cost, double volume) noexcept
	{
		if (cost == 0.0 || volume == 0.0)
		{
			return 0.0;
		}

		return cost / volume;
	}

	constexpr double calculate_fee(double amount, double feePercentage) noexcept
	{
		return amount * feePercentage * 0.01;
	}

	constexpr double calculate_trade_gain(double assetPrice, double tradeValue, double feePercentage, trade_action action) noexcept
	{
		int a = action == trade_action::SELL ? 0 : 1;
		return tradeValue * (1 - feePercentage * 0.01) * (a / assetPrice + assetPrice * (1 - a));
	}
}