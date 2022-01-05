#pragma once

#include <stdexcept>

#include "initialise/runner_initialise.h"

template<typename Strategy>
class Runner
{
private:
	RunMode _runMode;
	bool _initialised;
	Strategy _strategy;

public:
	template<typename... Args>
	explicit Runner(RunMode runMode, Args&&... args)
		: _runMode{ runMode }, _initialised{ false }, _strategy { std::forward<Args>(args)... }
	{
	}

	void initialise()
	{
		RunnerConfig runnerConfig = get_runner_config();
		TradingOptions tradingOptions = get_trading_options();
		std::vector<std::shared_ptr<Exchange>> exchanges = create_exchanges(runnerConfig, _runMode);

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