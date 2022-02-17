#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "exchanges/exchange_status.h"
#include "trading/order_book.h"
#include "trading/tradable_pair.h"
#include "common/types/result.h"

namespace cb::internal
{
	result<exchange_status> read_system_status(const std::string& jsonResult);
	result<std::vector<tradable_pair>> read_tradable_pairs(const std::string& jsonResult);
	result<order_book_state> read_order_book(const std::string& jsonResult, const tradable_pair& pair, int depth);
	result<std::unordered_map<asset_symbol, double>> read_balances(const std::string& jsonResult);
}
