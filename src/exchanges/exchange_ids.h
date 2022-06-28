#pragma once

#include <string_view>

namespace mb
{
	namespace exchange_ids
	{
		static constexpr std::string_view BACK_TEST = "BACK_TEST";

		static constexpr std::string_view KRAKEN = "kraken";
		static constexpr std::string_view COINBASE = "coinbase";
		static constexpr std::string_view BYBIT = "bybit";
		static constexpr std::string_view DIGIFINEX = "digifinex";

		constexpr std::vector<std::string_view> all()
		{
			return std::vector<std::string_view>
			{
				exchange_ids::KRAKEN,
				exchange_ids::COINBASE,
				exchange_ids::BYBIT,
				exchange_ids::DIGIFINEX
			};
		}
	}
}
