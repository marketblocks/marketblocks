#include "exchange.h"
#include "utils/containerutils.h"

const std::vector<TradablePair> Exchange::get_tradable_pairs() const
{
	return _marketData->get_tradable_pairs();
}

const std::unordered_map<TradablePair, OrderBookState> Exchange::get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const
{
	return _marketData->get_order_book(tradablePairs, depth);
}

const std::unordered_map<TradablePair, double> Exchange::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return _trader->get_fees(tradablePairs);
}

const std::unordered_map<AssetSymbol, double> Exchange::get_balances() const
{
	return _trader->get_balances();
}

void Exchange::trade(const TradeDescription& description)
{
	return _trader->trade(description);
}