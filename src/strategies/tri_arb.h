#pragma once

#include <memory>
#include <vector>

#include "trading/tradable_pair.h"
#include "trading/trading_constants.h"
#include "exchanges/exchange.h"
#include "runner/initialise/strategy_initialiser.h"

using namespace cb;

class sequence_step
{
private:
	tradable_pair _pair;
	trade_action _action;

public:
	constexpr sequence_step(tradable_pair pair, trade_action action)
		: _pair{ std::move(pair) }, _action{ std::move(action) }
	{}

	constexpr const tradable_pair& pair() const noexcept { return _pair; }
	constexpr const trade_action& action() const noexcept { return _action; }

	constexpr bool operator==(const sequence_step& other) const noexcept
	{
		return _action == other._action && _pair == other._pair;
	}
};

class tri_arb_sequence
{
private:
	sequence_step _first;
	sequence_step _middle;
	sequence_step _last;
	std::vector<tradable_pair> _pairs;

public:
	constexpr tri_arb_sequence(
		sequence_step first,
		sequence_step middle, 
		sequence_step last, 
		std::vector<tradable_pair> pairs)
		:
		_first{ std::move(first) }, 
		_middle{ std::move(middle) },
		_last{ std::move(last) }, 
		_pairs{ std::move(pairs) }
	{}

	constexpr const sequence_step& first() const noexcept { return _first; }
	constexpr const sequence_step& middle() const noexcept { return _middle; }
	constexpr const sequence_step& last() const noexcept { return _last; }
	constexpr const std::vector<cb::tradable_pair>& pairs() const noexcept { return _pairs; }
};

class tri_arb_spec
{
private:
	std::shared_ptr<exchange> _exchange;
	std::queue<tradable_pair> _orderBookMessageQueue;
	std::unordered_map<tradable_pair, std::vector<tri_arb_sequence>> _sequences;

public:
	tri_arb_spec(
		std::shared_ptr<exchange> exchange, 
		std::unordered_map<tradable_pair, std::vector<tri_arb_sequence>> sequences);

	std::shared_ptr<exchange> exchange() const { return _exchange; }
	std::queue<tradable_pair>& message_queue() { return _orderBookMessageQueue; }

	const std::vector<tri_arb_sequence>& get_sequences(const tradable_pair& pair) const;
};

class tri_arb_strategy
{
private:
	std::vector<tri_arb_spec> _specs;
	cb::trading_options _options;

public:
	tri_arb_strategy() {}

	void initialise(const cb::strategy_initialiser& initialiser);
	void run_iteration();
};

std::vector<tri_arb_spec> create_exchange_specs(const std::vector<std::shared_ptr<cb::exchange>>& exchanges, std::string_view fiatCurrency);