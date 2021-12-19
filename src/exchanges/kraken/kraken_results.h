#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "common/trading/order_book.h"
#include "common/trading/tradable_pair.h"

const std::vector<TradablePair> read_tradable_pairs(const std::string& jsonResult);
const OrderBookState read_order_book(const std::string& jsonResult, const TradablePair& pair, int depth);
const std::unordered_map<AssetSymbol, double> read_balances(const std::string& jsonResult);