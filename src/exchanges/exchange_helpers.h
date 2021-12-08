#pragma once

#include "exchange.h"

std::unordered_map<TradablePair, OrderBookLevel> get_best_order_book_prices(const Exchange& exchange, const std::vector<TradablePair>& tradablePairs);

double get_balance(const Exchange& exchange, const std::string& tickerId);