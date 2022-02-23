#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "exchanges/exchange_status.h"
#include "trading/order_book.h"
#include "trading/tradable_pair.h"
#include "trading/ticker_data.h"
#include "trading/order_description.h"
#include "common/types/result.h"

namespace cb::internal
{
	result<exchange_status> read_system_status(const std::string& jsonResult);
	result<std::vector<tradable_pair>> read_tradable_pairs(const std::string& jsonResult);
	result<ticker_data> read_ticker_data(const std::string& jsonResult);
	result<order_book_state> read_order_book(const std::string& jsonResult);
	result<std::unordered_map<asset_symbol, double>> read_balances(const std::string& jsonResult);
	result<double> read_fee(const std::string& jsonResult);
	result<std::vector<order_description>> read_open_orders(const std::string& jsonResult);
	result<std::vector<order_description>> read_closed_orders(const std::string& jsonResult);
	result<std::string> read_add_order(const std::string& jsonResult);
	result<void> read_cancel_order(const std::string& jsonResult);
}
