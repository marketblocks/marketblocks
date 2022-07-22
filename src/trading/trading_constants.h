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
			throw std::invalid_argument{ "Unknown order type" };
		}
	}
}
