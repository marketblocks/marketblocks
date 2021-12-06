#pragma once

#include "common/trading/tradable_pair.h"
#include "common/data/data.h"

class MarketData
{
public:
	virtual ~MarketData() = default;

	virtual const std::vector<TradablePair> get_tradable_pairs() const = 0;
	virtual const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const = 0;
};