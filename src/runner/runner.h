#pragma once

#include <stdexcept>

#include "runner_initialise.h"

template<typename Strategy>
class Runner
{
private:
	Strategy _strategy;
	bool _initialised;

public:
	template<typename... Args>
	explicit Runner(Args&&... args)
		: _strategy{ std::forward<Args>(args)... }, _initialised{ false }
	{
	}

	void initialise()
	{
		RunnerConfig runnerConfig = load_runner_config();
		TradingOptions tradingOptions = load_trading_options();
		std::vector<std::shared_ptr<Exchange>> exchanges = create_exchanges(runnerConfig);

		StrategyInitialiser strategyInitialiser
		{
			std::move(exchanges),
			std::move(tradingOptions)
		};

		_strategy.initialise(strategyInitialiser);

		_initialised = true;
	}

	void run()
	{
		if (!_initialised)
		{
			throw std::runtime_error("Runner must be initialized");
		}

		while (true)
		{
			_strategy.run_iteration();
		}
	}
};