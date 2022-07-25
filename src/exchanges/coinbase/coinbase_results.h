#pragma once

#include <vector>
#include <unordered_map>

#include "common/types/result.h"
#include "common/types/partial_data_result.h"
#include "trading/tradable_pair.h"
#include "trading/order_book.h"
#include "trading/tradable_pair.h"
#include "trading/ohlcv_data.h"
#include "trading/order_description.h"

namespace mb::coinbase
{
	result<std::vector<tradable_pair>> read_tradable_pairs(std::string_view jsonResult);
	result<std::vector<ohlcv_data>> read_ohlcv_data(std::string_view jsonResult, int count);
	result<double> read_price(std::string_view jsonResult);
	result<order_book_state> read_order_book(std::string_view jsonResult, int depth);
	result<double> read_fee(std::string_view jsonResult);
	result<std::unordered_map<std::string,double>> read_balances(std::string_view jsonResult);
	result<std::vector<order_description>> read_orders(std::string_view jsonResult);
	result<std::string> read_add_order(std::string_view jsonResult);
	result<void> read_cancel_order(std::string_view jsonResult);
}