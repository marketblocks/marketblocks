#pragma once

#include <vector>

#include "configs.h"
#include "exchanges/exchange.h"

class StrategyInitialiser
{
private:
	std::vector<std::shared_ptr<Exchange>> _exchanges;
	TradingOptions _options;

public:
	StrategyInitialiser(std::vector<std::shared_ptr<Exchange>> exchanges, TradingOptions options)
		: _exchanges{ std::move(exchanges) }, _options{ std::move(options) }
	{}

	std::vector<std::shared_ptr<Exchange>> exchanges() const { return _exchanges; }
	TradingOptions options() const { return _options; }
};