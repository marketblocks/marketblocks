#pragma once

#include "common/trading/tradable_pair.h"
#include "common/trading/order_book.h"

class MarketData
{
public:
	virtual ~MarketData() = default;

	virtual const std::vector<TradablePair> get_tradable_pairs() const = 0;
	virtual const std::unordered_map<TradablePair, OrderBookState> get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const = 0;
};