#pragma once

#include <memory>
#include <vector>

#include "misc/trading.h"
#include "misc/options.h"
#include "exchanges/exchange.h"
#include "exchanges/paper_trader/paper_trader.h"


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

	bool operator==(const SequenceStep& other) const
	{
		return _action == other._action && _pair == other._pair;
	}
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

class TriArbSequenceTradeStep
{
private:
	TradeDescription _description;
	double _gainValue;

public:
	TriArbSequenceTradeStep(TradeDescription description, double gainValue)
		: _description{std::move(description)}, _gainValue{gainValue}
	{}

	TradeDescription description() const { return _description; }
	double gain_value() const { return _gainValue; }
};

class TriArbStrategy
{
private:
	std::vector<TriArbExchangeSpec> _specs;
	TradingOptions _options;
	PaperTrader simulator;

	TriArbSequenceTradeStep create_sequence_trade_step(const SequenceStep& sequenceStep, const PriceData& priceData, double fee, double previousTradeGain);
	TriArbSequenceTrades calculate_trades(const TriArbSequence& sequence, const std::unordered_map<TradablePair, PriceData>& prices, const std::unordered_map<TradablePair, double>& fees, double initialTradeCost);
	double calculate_profit(const TriArbSequenceTrades& trades);

public:
	explicit TriArbStrategy(std::vector<TriArbExchangeSpec> specs, TradingOptions options)
		: _specs{ std::move(specs) }, _options{ std::move(options) }, simulator{ 0.26 }
	{}

	void run_iteration();
};

std::vector<TriArbExchangeSpec> create_exchange_specs(const std::vector<std::shared_ptr<Exchange>>& exchanges, const std::string& fiatCurrency);
TriArbStrategy create_tri_arb_strategy(const std::vector<std::shared_ptr<Exchange>>& exchanges, TradingOptions options);