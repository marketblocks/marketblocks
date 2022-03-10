#include "exchange_helpers.h"
#include "common/utils/containerutils.h"

namespace cb
{
	std::unordered_map<tradable_pair, order_book_level> get_best_order_book_prices(const websocket_stream& websocketStream, const std::vector<tradable_pair>& tradablePairs)
	{
		return to_unordered_map<tradable_pair, order_book_level>(
			tradablePairs,
			[](const tradable_pair& pair) { return pair; },
			[&websocketStream](const tradable_pair& pair) { return websocketStream.get_order_book_snapshot(pair).level(0); });
	}

	double get_balance(const exchange& exchange, std::string_view tickerId)
	{
		unordered_string_map<double> balances{ exchange.get_balances() };

		auto it = balances.find(tickerId);
		if (it != balances.end())
		{
			return it->second;
		}

		return 0.0;
	}
}