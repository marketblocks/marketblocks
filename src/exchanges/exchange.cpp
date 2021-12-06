#include "exchange.h"

const std::vector<TradablePair> Exchange::get_tradable_pairs() const
{
	return _marketData->get_tradable_pairs();
}

const std::unordered_map<TradablePair, PriceData> Exchange::get_price_data(const std::vector<TradablePair>& tradablePairs) const
{
	return _marketData->get_price_data(tradablePairs);
}

double Exchange::get_fee(const TradablePair& tradablePair) const
{
	return _trader->get_fee(tradablePair);
}

const std::unordered_map<TradablePair, double> Exchange::get_fees(const std::vector<TradablePair>& tradablePairs) const
{
	return _trader->get_fees(tradablePairs);
}

const std::unordered_map<std::string, double> Exchange::get_account_balance() const
{
	return _trader->get_all_balances();
}

double Exchange::get_balance(const std::string& tickerId) const
{
	return _trader->get_balance(tickerId);
}

void Exchange::trade(const TradeDescription& description)
{
	return _trader->trade(description);
}