#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "market_data.h"
#include "trader.h"

class Exchange final
{
private:
	std::unique_ptr<MarketData> _marketData;
	std::unique_ptr<Trader> _trader;

public:
	Exchange(std::unique_ptr<MarketData> marketData, std::unique_ptr<Trader> trader)
		: _marketData{std::move(marketData)}, _trader{std::move(trader)}
	{}

	const std::vector<TradablePair> get_tradable_pairs() const;
	const std::unordered_map<TradablePair, PriceData> get_price_data(const std::vector<TradablePair>& tradablePairs) const;

	double get_fee(const TradablePair& tradablePair) const;
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const;
	const std::unordered_map<std::string, double> get_account_balance() const;
	double get_balance(const std::string& tickerId) const;
	void trade(const TradeDescription& description);
};