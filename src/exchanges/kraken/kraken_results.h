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
	result<exchange_status> read_system_status(std::string_view jsonResult);
	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult);
	result<ticker_data> read_ticker_data(std::string_view jsonResult);
	result<order_book_state> read_order_book(std::string_view jsonResult);
	result<std::unordered_map<asset_symbol, double>> read_balances(std::string_view jsonResult);
	result<double> read_fee(std::string_view jsonResult);
	result<std::vector<order_description>> read_open_orders(std::string_view jsonResult);
	result<std::vector<order_description>> read_closed_orders(std::string_view jsonResult);
	result<std::string> read_add_order(std::string_view jsonResult);
	result<void> read_cancel_order(std::string_view jsonResult);
}
