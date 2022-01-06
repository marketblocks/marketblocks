#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "trading/tradable_pair.h"
#include "trading/asset_symbol.h"
#include "trading/trading_constants.h"
#include "trading/order_book.h"
#include "trading/trade_description.h"
#include "paper_trading/paper_trader.h"

#include "exchange_id.h"

class Exchange
{
private:
	ExchangeId _id;

public:
	Exchange(ExchangeId id)
		: _id{ id }
	{}

	virtual ~Exchange() = default;

	ExchangeId id() const { return _id; }

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
	std::unique_ptr<MarketApi> _marketApi;
	std::unique_ptr<TradeApi> _tradeApi;

public:
	MultiComponentExchange(ExchangeId id, std::unique_ptr<MarketApi> dataApi, std::unique_ptr<TradeApi> tradeApi)
		: Exchange{ id }, _marketApi{ std::move(dataApi) }, _tradeApi{std::move(tradeApi)}
	{}

	virtual const std::vector<TradablePair> get_tradable_pairs() const override
	{
		return _marketApi->get_tradable_pairs();
	}

	virtual const std::unordered_map<TradablePair, OrderBookState> get_order_book(const std::vector<TradablePair>& tradablePairs, int depth) const override
	{
		return _marketApi->get_order_book(tradablePairs, depth);
	}

	virtual const std::unordered_map<AssetSymbol, double> get_balances() const override
	{
		return _tradeApi->get_balances();
	}

	virtual const std::unordered_map<TradablePair, double> get_fees(const std::vector<TradablePair>& tradablePairs) const override
	{
		return _tradeApi->get_fees(tradablePairs);
	}

	virtual TradeResult trade(const TradeDescription& description) override
	{
		return _tradeApi->trade(description);
	}
};

typedef MultiComponentExchange<Exchange, PaperTrader> LiveTestExchange;