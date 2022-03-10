#pragma once

#include <memory>
#include <vector>

#include "trading/tradable_pair.h"
#include "trading/trading_constants.h"
#include "exchanges/exchange.h"
#include "runner/initialise/strategy_initialiser.h"

class sequence_step
{
private:
	cb::tradable_pair _pair;
	cb::trade_action _action;

public:
	explicit sequence_step(cb::tradable_pair pair, cb::trade_action action);

	const cb::tradable_pair& pair() const { return _pair; }
	const cb::trade_action& action() const { return _action; }

	bool operator==(const sequence_step& other) const;
};

class tri_arb_sequence
{
private:
	sequence_step _first;
	sequence_step _middle;
	sequence_step _last;
	std::vector<cb::tradable_pair> _pairs;

public:
	explicit tri_arb_sequence(sequence_step first, sequence_step middle, sequence_step last, std::vector<cb::tradable_pair> pairs);

	const sequence_step& first() const { return _first; }
	const sequence_step& middle() const { return _middle; }
	const sequence_step& last() const { return _last; }
	const std::vector<cb::tradable_pair>& pairs() const { return _pairs; }
};

class tri_arb_exchange_spec
{
private:
	std::shared_ptr<cb::exchange> _exchange;
	std::vector<tri_arb_sequence> _sequences;

public:
	explicit tri_arb_exchange_spec(std::shared_ptr<cb::exchange> exchange, std::vector<tri_arb_sequence> sequences);

	cb::exchange& exchange() const { return *(_exchange.get()); }
	const std::vector<tri_arb_sequence>& sequences() const { return _sequences; }
};

class tri_arb_strategy
{
private:
	std::vector<tri_arb_exchange_spec> _specs;
	cb::trading_options _options;

public:
	tri_arb_strategy() {}

	void initialise(const cb::strategy_initialiser& initialiser);
	void run_iteration();
};

std::vector<tri_arb_exchange_spec> create_exchange_specs(const std::vector<std::shared_ptr<cb::exchange>>& exchanges, std::string_view fiatCurrency);