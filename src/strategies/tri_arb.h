#pragma once

#include <memory>
#include <vector>

#include "common/trading/tradable_pair.h"
#include "common/trading/trading_constants.h"
#include "common/trading/trading_options.h"
#include "exchanges/exchange.h"

class SequenceStep
{
private:
	TradablePair _pair;
	TradeAction _action;

public:
	explicit SequenceStep(TradablePair pair, TradeAction action);

	const TradablePair& pair() const { return _pair; }
	const TradeAction& action() const { return _action; }

	bool operator==(const SequenceStep& other) const;
};

class TriArbSequence
{
private:
	SequenceStep _first;
	SequenceStep _middle;
	SequenceStep _last;
	std::vector<TradablePair> _pairs;

public:
	explicit TriArbSequence(SequenceStep first, SequenceStep middle, SequenceStep last, std::vector<TradablePair> pairs);

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
	explicit TriArbExchangeSpec(std::shared_ptr<Exchange> exchange, std::vector<TriArbSequence> sequences);

	Exchange& exchange() const { return *(_exchange.get()); }
	const std::vector<TriArbSequence>& sequences() const { return _sequences; }
};

class TriArbStrategy
{
private:
	std::vector<TriArbExchangeSpec> _specs;
	TradingOptions _options;

public:
	explicit TriArbStrategy(std::vector<TriArbExchangeSpec> specs, TradingOptions options);

	void run_iteration();
};

std::vector<TriArbExchangeSpec> create_exchange_specs(const std::vector<std::shared_ptr<Exchange>>& exchanges, const std::string& fiatCurrency);
TriArbStrategy create_tri_arb_strategy(const std::vector<std::shared_ptr<Exchange>>& exchanges, TradingOptions options);