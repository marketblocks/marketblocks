#pragma once

#include <string_view>

namespace mb
{
	namespace exchange_ids
	{
		static constexpr std::string_view KRAKEN = "kraken";
		static constexpr std::string_view COINBASE = "coinbase";

		constexpr std::vector<std::string_view> all()
		{
			return std::vector<std::string_view>
			{
				exchange_ids::KRAKEN,
				exchange_ids::COINBASE
			};
		}
	}
}
