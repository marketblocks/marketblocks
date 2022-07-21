#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "exchanges/exchange_status.h"
#include "trading/order_book.h"
#include "trading/tradable_pair.h"
#include "trading/ohlcv_data.h"
#include "trading/order_description.h"
#include "common/types/result.h"
#include "common/types/partial_data_result.h"
#include "common/types/unordered_string_map.h"

namespace mb::binance
{
	result<exchange_status> read_system_status(std::string_view jsonResult);
	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult);
	result<std::vector<ohlcv_data>> read_ohlcv(std::string_view jsonResult);
	result<double> read_price(std::string_view jsonResult);
	result<std::unordered_map<std::string, double>> read_prices(std::string_view jsonResult);
	result<order_book_state> read_order_book(std::string_view jsonResult);
	result<unordered_string_map<double>> read_balances(std::string_view jsonResult);
	result<double> read_fee(std::string_view jsonResult);
	result<std::vector<order_description>> read_open_orders(std::string_view jsonResult);
	result<std::vector<order_description>> read_closed_orders(std::string_view jsonResult);
	result<std::string> read_add_order(std::string_view jsonResult);
	result<void> read_cancel_order(std::string_view jsonResult);
}
