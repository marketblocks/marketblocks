#pragma once

#include <string_view>

namespace AssetSymbol
{
	constexpr std::string_view GBP{ "GBP" };
	constexpr std::string_view USD{ "USD" };
	constexpr std::string_view BTC{ "BTC" };
	constexpr std::string_view ETH{ "ETH" };
	constexpr std::string_view LTC{ "LTC" };
}

enum class TradeAction
{
	BUY,
	SELL
};