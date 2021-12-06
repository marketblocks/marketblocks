#include <gmock/gmock.h>

#include "exchanges/exchange.h"

class MockMarketData : public MarketData
{
public:
	MOCK_METHOD(const std::vector<TradablePair>, get_tradable_pairs, (), (const, override));
	MOCK_METHOD((const std::unordered_map<TradablePair, PriceData>), get_price_data, (const std::vector<TradablePair>& tradablePairs), (const, override));
};

class MockTrader : public Trader
{
public:

	MOCK_METHOD(double, get_fee, (const TradablePair& tradablePair), (const, override));
	MOCK_METHOD((const std::unordered_map<TradablePair, double>), get_fees, (const std::vector<TradablePair>& tradablePairs), (const, override));
	MOCK_METHOD((const std::unordered_map<std::string, double>), get_all_balances, (), (const, override));
	MOCK_METHOD(double, get_balance, (const std::string& tickerId), (const, override));
	MOCK_METHOD(void, trade, (const TradeDescription& description), (override));
};