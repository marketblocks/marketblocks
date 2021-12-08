#include "exchange_helpers.h"
#include "utils/containerutils.h"

std::unordered_map<TradablePair, OrderBookLevel> get_best_order_book_prices(const Exchange& exchange, const std::vector<TradablePair>& tradablePairs)
{
	std::unordered_map<TradablePair, OrderBookState> orderBook = exchange.get_order_book(tradablePairs, 1);

	return to_unordered_map<TradablePair, OrderBookLevel>(
		tradablePairs,
		[](const TradablePair& pair) { return pair; },
		[&orderBook](const TradablePair& pair) { return orderBook.at(pair).level(0); });
}

double get_balance(const Exchange& exchange, const std::string& tickerId)
{
	return exchange.get_balances().at(tickerId);
}