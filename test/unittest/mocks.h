#pragma once

#include <gmock/gmock.h>
#include <vector>

#include "exchanges/exchange.h"

namespace cb::test
{
	class mock_exchange : public exchange
	{
	public:
		MOCK_METHOD(const std::vector<TradablePair>, get_tradable_pairs, (), (const, override));
		MOCK_METHOD((const std::unordered_map<TradablePair, OrderBookState>), get_order_book, (const std::vector<TradablePair>& tradablePairs, int depth), (const, override));
		MOCK_METHOD((const std::unordered_map<AssetSymbol, double>), get_balances, (), (const, override));
		MOCK_METHOD((const std::unordered_map<TradablePair, double>), get_fees, (const std::vector<TradablePair>& tradablePairs), (const, override));
		MOCK_METHOD(TradeResult, trade, (const TradeDescription& description), (override));
	};
}
