#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "exchanges/exchange_status.h"
#include "trading/order_book.h"
#include "trading/tradable_pair.h"

namespace cb::internal
{
	exchange_status read_system_status(const std::string& jsonResult);
	const std::vector<tradable_pair> read_tradable_pairs(const std::string& jsonResult);
	const order_book_state read_order_book(const std::string& jsonResult, const tradable_pair& pair, int depth);
	const std::unordered_map<asset_symbol, double> read_balances(const std::string& jsonResult);
}
