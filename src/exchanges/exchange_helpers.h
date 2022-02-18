#pragma once

#include "exchange.h"

namespace cb
{
	std::unordered_map<tradable_pair, order_book_level> get_best_order_book_prices(const websocket_stream& websocketStream, const std::vector<tradable_pair>& tradablePairs);

	double get_balance(const exchange& exchange, const asset_symbol& tickerId);
}