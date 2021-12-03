#pragma once

#include <memory>
#include <vector>

#include "misc/trading.h"
#include "misc/options.h"
#include "exchanges/exchange.h"


class SequenceStep
{
private:
	TradablePair _pair;
	TradeAction _action;

public:
	explicit SequenceStep(TradablePair pair, TradeAction action)
		: _pair{ std::move(pair) }, _action{ std::move(action) }
	{}

	const TradablePair& pair() const { return _pair; }
	const TradeAction& action() const { return _action; }
};

class TriArbSequence
{
private:
	SequenceStep _first;
	SequenceStep _middle;
	SequenceStep _last;
	std::vector<TradablePair> _pairs;

public:
	explicit TriArbSequence(SequenceStep first, SequenceStep middle, SequenceStep last, std::vector<TradablePair> pairs)
		: _first{std::move(first)}, _middle{std::move(middle)}, _last{std::move(last)}, _pairs{std::move(pairs)}
	{}

	const SequenceStep& first() const { return _first; }
	const SequenceStep& middle() const { return _middle; }
	const SequenceStep& last() const { return _last; }
	const std::vector<TradablePair>& pairs() const { return _pairs; }
};

class TriArbExchangeSpec
{
private:
	std::shared_ptr<Exchange> _exchange;
	std::vector<TriArbSequence> _sequences;

public:
	explicit TriArbExchangeSpec(std::shared_ptr<Exchange> exchange, std::vector<TriArbSequence> sequences)
		: _exchange{exchange}, _sequences{std::move(sequences)}
	{}

	Exchange& exchange() const { return *(_exchange.get()); }
	const std::vector<TriArbSequence>& sequences() const { return _sequences; }
};

class TriArbSequenceTrades
{
private:
	TradeDescription _first;
	TradeDescription _middle;
	TradeDescription _last;

public:
	TriArbSequenceTrades(TradeDescription first, TradeDescription middle, TradeDescription last)
		: _first{ std::move(first) }, _middle{ std::move(middle) }, _last{ std::move(last) }
	{}

	const TradeDescription& first() const { return _first; }
	const TradeDescription& middle() const { return _middle; }
	const TradeDescription& last() const { return _last; }
};

class TriArbStrategy
{
private:
	std::vector<TriArbExchangeSpec> _specs;
	TradingOptions _options;

	TriArbSequenceTrades calculate_trades(const TriArbSequence& sequence, const std::unordered_map<TradablePair, PriceData>& prices, double initialTradeCost);

public:
	explicit TriArbStrategy(std::vector<TriArbExchangeSpec> specs, TradingOptions options)
		: _specs{ std::move(specs) }, _options{ std::move(options) }
	{}

	void run_iteration();
};

TriArbStrategy create_tri_arb_strategy(const std::vector<std::shared_ptr<Exchange>>& exchanges, TradingOptions options);