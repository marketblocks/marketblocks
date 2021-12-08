#pragma once

#include <gmock/gmock.h>

#include "exchanges/market_data.h"
#include "exchanges/trader.h"

class MockMarketData : public MarketData
{
public:
	MOCK_METHOD(const std::vector<TradablePair>, get_tradable_pairs, (), (const, override));
	MOCK_METHOD((const std::unordered_map<TradablePair, OrderBookState>), get_order_book, (const std::vector<TradablePair>& tradablePairs, int depth), (const, override));
};

class MockTrader : public Trader
{
public:

	MOCK_METHOD((const std::unordered_map<TradablePair, double>), get_fees, (const std::vector<TradablePair>& tradablePairs), (const, override));
	MOCK_METHOD((const std::unordered_map<std::string, double>), get_balances, (), (const, override));
	MOCK_METHOD(void, trade, (const TradeDescription& description), (override));
};