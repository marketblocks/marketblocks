#pragma once

#include <string_view>
#include <vector>

namespace mb
{
	namespace exchange_ids
	{
		static constexpr std::string_view BACK_TEST = "BACK_TEST";

		static constexpr std::string_view KRAKEN = "kraken";
		static constexpr std::string_view COINBASE = "coinbase";
		static constexpr std::string_view BYBIT = "bybit";
		static constexpr std::string_view DIGIFINEX = "digifinex";
		static constexpr std::string_view DEXTRADE = "dextrade";
		static constexpr std::string_view BINANCE = "binance";

		std::vector<std::string_view> all();
	}
}
