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

	// Market Data
	const std::vector<TradablePair> get_tradable_pairs() const;
	const std::unordered_map<TradablePair, OrderBookState> get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const;

	// Trader
	const std::unordered_map<std::string, double> get_balances() const;
	const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const;

	void trade(const TradeDescription& description);
};