#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "common/trading/tradable_pair.h"
#include "common/trading/asset_symbol.h"
#include "common/trading/trading_constants.h"
#include "common/trading/order_book.h"
#include <common/trading/trade_description.h>

class Exchange
{
public:
	virtual ~Exchange() = default;

	virtual const std::vector<TradablePair> get_tradable_pairs() const = 0;
	virtual const std::unordered_map<TradablePair, OrderBookState> get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const = 0;
	virtual const std::unordered_map<AssetSymbol, double> get_balances() const = 0;
	virtual const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const = 0;
	virtual TradeResult trade(const TradeDescription& description) = 0;
};

template<typename MarketApi, typename TradeApi>
class MultiComponentExchange final : public Exchange
{
private:
	MarketApi _marketApi;
	TradeApi _tradeApi;

public:
	MultiComponentExchange(MarketApi&& dataApi, TradeApi&& tradeApi)
		: _marketApi{ std::forward<MarketApi>(dataApi) }, _tradeApi{ std::forward<TradeApi>(tradeApi) }
	{}

	virtual const std::vector<TradablePair> get_tradable_pairs() const override
	{
		return _marketApi.get_tradable_pairs();
	}

	virtual const std::unordered_map<TradablePair, OrderBookState> get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const override
	{
		return _marketApi.get_order_book(tradablePairs, depth);
	}

	virtual const std::unordered_map<AssetSymbol, double> get_balances() const override
	{
		return _tradeApi.get_balances();
	}

	virtual const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override
	{
		return _tradeApi.get_fees(tradablePairs);
	}

	virtual TradeResult trade(const TradeDescription& description) override
	{
		return _tradeApi.trade(description);
	}
};