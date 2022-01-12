#include "exchange_helpers.h"
#include "common/utils/containerutils.h"

std::unordered_map<TradablePair, OrderBookLevel> get_best_order_book_prices(const Exchange& exchange, const std::vector<TradablePair>& tradablePairs)
{
	std::unordered_map<TradablePair, OrderBookState> orderBook = exchange.get_order_book(tradablePairs, 1);

	return to_unordered_map<TradablePair, OrderBookLevel>(
		tradablePairs,
		[](const TradablePair& pair) { return pair; },
		[&orderBook](const TradablePair& pair) { return orderBook.at(pair).level(0); });
}

std::unordered_map<TradablePair, OrderBookLevel> get_best_order_book_prices(const WebsocketStream& websocketStream, const std::vector<TradablePair>& tradablePairs)
{
	return to_unordered_map<TradablePair, OrderBookLevel>(
		tradablePairs,
		[](const TradablePair& pair) { return pair; },
		[&websocketStream](const TradablePair& pair) { return websocketStream.get_current_order_book(pair).level(0); });
}

double get_balance(const Exchange& exchange, const AssetSymbol& tickerId)
{
	return exchange.get_balances().at(tickerId);
}