#include "exchange_ids.h"

namespace mb::exchange_ids
{
	std::vector<std::string_view> all()
	{
		return std::vector<std::string_view>
		{
			exchange_ids::KRAKEN,
			exchange_ids::COINBASE,
			exchange_ids::BYBIT,
			exchange_ids::DIGIFINEX,
			exchange_ids::BINANCE
		};
	}
}