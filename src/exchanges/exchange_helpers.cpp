#include "exchange_helpers.h"
#include "common/utils/containerutils.h"

namespace cb
{
	std::unordered_map<tradable_pair, order_book_level> get_best_order_book_prices(const exchange& exchange, const std::vector<tradable_pair>& tradablePairs)
	{
		std::unordered_map<tradable_pair, order_book_state> orderBook = exchange.get_order_book(tradablePairs, 1);

		return to_unordered_map<tradable_pair, order_book_level>(
			tradablePairs,
			[](const tradable_pair& pair) { return pair; },
			[&orderBook](const tradable_pair& pair) { return orderBook.at(pair).level(0); });
	}

	std::unordered_map<tradable_pair, order_book_level> get_best_order_book_prices(const websocket_stream& websocketStream, const std::vector<tradable_pair>& tradablePairs)
	{
		return to_unordered_map<tradable_pair, order_book_level>(
			tradablePairs,
			[](const tradable_pair& pair) { return pair; },
			[&websocketStream](const tradable_pair& pair) { return websocketStream.get_order_book_snapshot(pair).level(0); });
	}

	double get_balance(const exchange& exchange, const asset_symbol& tickerId)
	{
		return exchange.get_balances().at(tickerId);
	}
}