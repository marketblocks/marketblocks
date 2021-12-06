#pragma once

#include <string>

#include "common/trading/tradable_pair.h"
#include "common/trading/trade_description.h"

class Trader
{
public:
	virtual ~Trader() = default;

	virtual double get_fee(const TradablePair& tradablePair) const = 0;
	virtual const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const = 0;
	virtual const std::unordered_map<std::string, double> get_all_balances() const = 0;
	virtual double get_balance(const std::string& tickerId) const = 0;
	virtual void trade(const TradeDescription& description) = 0;
};