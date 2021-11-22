#pragma once

#include <vector>
#include <unordered_map>

#include "..\data.h"
#include "..\trading.h"

class Exchange
{
public:
	virtual ~Exchange() = default;

	virtual double get_fee() const = 0;
	virtual const std::vector<TradablePair> get_tradable_pairs() const = 0;
	virtual const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const = 0;
};