#pragma once

#include "exchange.h"

class DummyExchange final : public Exchange
{
	virtual double get_fee() const override;
	virtual const std::vector<TradablePair> get_tradable_pairs() const override;
	virtual const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const override;
};
