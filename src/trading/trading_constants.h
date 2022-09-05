#pragma once

#include <stdexcept>
#include <string_view>

namespace mb
{
	enum class trade_action
	{
		SELL = 0,
		BUY = 1
	};

	enum class order_status
	{
		OPEN,
		CLOSED,
		PARTIALLY_FILLED,
		CANCELLED
	};

	enum class order_type
	{
		MARKET,
		LIMIT,
		STOP_LOSS,
		TRAILING_STOP_LOSS,
		TAKE_PROFIT
	};

	constexpr std::string_view to_string(order_type orderType)
	{
		constexpr std::string_view MARKET = "MARKET";
		constexpr std::string_view LIMIT = "LIMIT";
		constexpr std::string_view STOP_LOSS = "STOP_LOSS";
		constexpr std::string_view TRAILING_STOP_LOSS = "TRAILING_STOP_LOSS";
		constexpr std::string_view TAKE_PROFIT = "TAKE_PROFIT";

		switch (orderType)
		{
		case order_type::MARKET:
			return MARKET;
		case order_type::LIMIT:
			return LIMIT;
		case order_type::STOP_LOSS:
			return STOP_LOSS;
		case order_type::TRAILING_STOP_LOSS:
			return TRAILING_STOP_LOSS;
		case order_type::TAKE_PROFIT:
			return TAKE_PROFIT;
		default:
			throw std::invalid_argument{ "Unknown order type" };
		}
	}

	constexpr std::string_view to_string(trade_action tradeAction)
	{
		constexpr std::string_view BUY = "buy";
		constexpr std::string_view SELL = "sell";

		switch (tradeAction)
		{
		case trade_action::BUY:
			return BUY;
		case trade_action::SELL:
			return SELL;
		default:
			throw std::invalid_argument{ "Unknown trade action" };
		}
	}
}
